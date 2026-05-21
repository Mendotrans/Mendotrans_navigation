#include "public_transport_system.h"
#include "PublicTransportSystem/mendotran_types.h"
#include "Renderer/graph_renderer.h"
#include <stdexcept>

namespace mendotran {

PublicTransportSystem::PublicTransportSystem(const std::string &db_path,
                                             const ApiConfig &cfg)
    : m_cfg(cfg), m_db(db_path) {
  m_db.init_schema();
}

PublicTransportSystem::PublicTransportSystem(const std::string &db_path)
    : m_db(db_path) {
  if (!m_db.is_populated()) {
    throw std::runtime_error("No db found!");
  }
}

std::unique_ptr<httplib::Client> PublicTransportSystem::make_client() const {
  auto client = std::make_unique<httplib::Client>(
      (m_cfg.use_https ? "https://" : "http://") + m_cfg.base_url);
  client->set_connection_timeout(10);
  client->set_read_timeout(10);
  return client;
}

nlohmann::json PublicTransportSystem::post(const std::string &endpoint,
                                           const nlohmann::json &body) const {
  const std::string path = m_cfg.base_path + "/" + endpoint;
  const std::string body_str = body.dump();

  httplib::Headers headers = {
      {"Accept", "*/*"},
      {"Accept-Language", "es-AR,es;q=0.8,en-US;q=0.5,en;q=0.3"},
      {"Origin", (m_cfg.use_https ? "https://" : "http://") + m_cfg.base_url},
      {"Referer", (m_cfg.use_https ? "https://" : "http://") + m_cfg.base_url +
                      "/web/mendoza/"},
      {"User-Agent", "cpp-httplib/transit-client"},
  };

  auto client = make_client();
  auto res = client->Post(path, headers, body_str, "application/json");

  if (!res)
    throw std::runtime_error("HTTP POST failed: " + path);
  if (res->status != 200)
    throw std::runtime_error("HTTP " + std::to_string(res->status) + " from " +
                             path);

  return nlohmann::json::parse(res->body);
}

nlohmann::json PublicTransportSystem::api_fetch_stops() const {
  std::cout << "Fetching stops data" << '\n';
  return post("search", {
                            {"token", m_cfg.token},
                            {"text", ""},
                            {"xss", m_cfg.xss_search},
                            {"search", {"stops"}},
                            {"no_favorites", true},
                        });
}

nlohmann::json PublicTransportSystem::api_fetch_services_list() const {
  std::cout << "Fetching service_list data" << '\n';
  return post("search", {
                            {"token", m_cfg.token},
                            {"text", ""},
                            {"xss", m_cfg.xss_search},
                            {"search", {"services"}},
                            {"no_favorites", true},
                        });
}

nlohmann::json
PublicTransportSystem::api_fetch_stop_arrivals(int stop_id) const {
  std::cout << "Fetching arrivals data" << '\n';
  return post("arrivals", {
                              {"token", m_cfg.token},
                              {"stop_id", stop_id},
                              {"first_time", false},
                              {"xss", m_cfg.xss_arrivals},
                          });
}

nlohmann::json
PublicTransportSystem::api_fetch_service_detail(int service_id) const {
  std::cout << "Fetching service_detail data" << '\n';
  return post("service", {
                             {"token", m_cfg.token},
                             {"service_id", service_id},
                             {"encode_polyline", true},
                             {"vehicles", true},
                             {"xss", m_cfg.xss_service},
                         });
}

std::vector<Stop>
PublicTransportSystem::parse_stops(const nlohmann::json &raw) {
  std::vector<Stop> stops;
  for (const auto &s : raw.at("search")) {
    std::cout << "Stop: " << s << '\n';
    stops.push_back({
        s.at("stop_id").get<int>(),
        s.at("type").get<std::string>(),
        s.at("coordinates")[0].get<double>(),
        s.at("coordinates")[1].get<double>(),
        s.value("code", ""),
        s["location"].is_null() ? "" : s["location"].get<std::string>(),
    });
  }
  return stops;
}

std::vector<Service>
PublicTransportSystem::parse_services(const nlohmann::json &raw) {
  std::vector<Service> services;
  for (const auto &s : raw.at("search")) {
    services.push_back({
        s.at("service_id").get<int>(),
        s.at("type").get<std::string>(),
        s.at("group_id").get<int>(),
        s.value("code", ""),
        s.value("name", ""),
        s.value("color", ""),
        s.value("mode", ""),
    });
  }
  return services;
}

std::vector<Group>
PublicTransportSystem::parse_groups(const nlohmann::json &raw) {
  std::vector<Group> groups;
  for (const auto &[gid, meta] : raw.at("groups").items())
    groups.push_back({gid, meta.value("name", "")});
  return groups;
}

void PublicTransportSystem::init_static_data() {
  if (m_db.is_populated())
    throw std::runtime_error(
        "DB already populated. Use force_reinit() to overwrite.");

  auto stops_raw = api_fetch_stops();
  auto services_raw = api_fetch_services_list();

  m_db.insert_stops(parse_stops(stops_raw));
  m_db.insert_services(parse_services(services_raw));
  m_db.insert_groups(parse_groups(services_raw));
}

void PublicTransportSystem::force_reinit() {
  m_db.clear_data();
  init_static_data();
}

std::vector<Stop> PublicTransportSystem::search_stops(const std::string &query,
                                                      int limit) const {
  return m_db.search_stops(query, limit);
}

std::vector<Service>
PublicTransportSystem::search_services(const std::string &query,
                                       int limit) const {
  return m_db.search_services(query, limit);
}

std::vector<int> PublicTransportSystem::all_service_ids() const {
  return m_db.all_service_ids();
}

nlohmann::json PublicTransportSystem::fetch_arrivals(int stop_id) const {
  return api_fetch_stop_arrivals(stop_id);
}

nlohmann::json PublicTransportSystem::fetch_service_detail(int service_id,
                                                           bool use_cache) {
  if (use_cache) {
    auto cached = m_db.get_service_detail(service_id);
    if (cached)
      return *cached;
  }
  auto data = api_fetch_service_detail(service_id);
  m_db.upsert_service_detail(service_id, data);
  return data;
}

void PublicTransportSystem::fetch_all_service_details(
    std::chrono::milliseconds delay, bool force) {
  for (int sid : m_db.all_service_ids()) {
    if (!force && m_db.get_service_detail(sid))
      continue;
    auto data = api_fetch_service_detail(sid);
    m_db.upsert_service_detail(sid, data);
    std::this_thread::sleep_for(delay);
  }
}

} // namespace mendotran
