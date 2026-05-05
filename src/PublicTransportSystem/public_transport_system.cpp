#include "public_transport_system.h"

namespace mendotran {

inline PublicTransportSystem::PublicTransportSystem(const std::string &db_path,
                                                    const ApiConfig &cfg)
    : cfg_(cfg), db_(db_path) {
  db_.init_schema();
}

inline std::unique_ptr<httplib::Client>
PublicTransportSystem::make_client() const {
  auto client = std::make_unique<httplib::Client>(
      (cfg_.use_https ? "https://" : "http://") + cfg_.base_url);
  client->set_connection_timeout(10);
  client->set_read_timeout(10);
  return client;
}

inline nlohmann::json
PublicTransportSystem::post(const std::string &endpoint,
                            const nlohmann::json &body) const {
  const std::string path = cfg_.base_path + "/" + endpoint;
  const std::string body_str = body.dump();

  httplib::Headers headers = {
      {"Accept", "*/*"},
      {"Accept-Language", "es-AR,es;q=0.8,en-US;q=0.5,en;q=0.3"},
      {"Origin", (cfg_.use_https ? "https://" : "http://") + cfg_.base_url},
      {"Referer", (cfg_.use_https ? "https://" : "http://") + cfg_.base_url +
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

inline nlohmann::json PublicTransportSystem::api_fetch_stops() const {
  return post("search", {
                            {"token", cfg_.token},
                            {"text", ""},
                            {"xss", cfg_.xss_search},
                            {"search", {"stops"}},
                            {"no_favorites", true},
                        });
}

inline nlohmann::json PublicTransportSystem::api_fetch_services_list() const {
  return post("search", {
                            {"token", cfg_.token},
                            {"text", ""},
                            {"xss", cfg_.xss_search},
                            {"search", {"services"}},
                            {"no_favorites", true},
                        });
}

inline nlohmann::json
PublicTransportSystem::api_fetch_stop_arrivals(int stop_id) const {
  return post("arrivals", {
                              {"token", cfg_.token},
                              {"stop_id", stop_id},
                              {"first_time", false},
                              {"xss", cfg_.xss_arrivals},
                          });
}

inline nlohmann::json
PublicTransportSystem::api_fetch_service_detail(int service_id) const {
  return post("service", {
                             {"token", cfg_.token},
                             {"service_id", service_id},
                             {"encode_polyline", true},
                             {"vehicles", true},
                             {"xss", cfg_.xss_service},
                         });
}

inline std::vector<Stop>
PublicTransportSystem::parse_stops(const nlohmann::json &raw) {
  std::vector<Stop> stops;
  for (const auto &s : raw.at("search")) {
    stops.push_back({
        s.at("stop_id").get<int>(),
        s.at("type").get<std::string>(),
        s.at("coordinates")[0].get<double>(),
        s.at("coordinates")[1].get<double>(),
        s.value("code", ""),
        s.value("location", ""),
    });
  }
  return stops;
}

inline std::vector<Service>
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

inline std::vector<Group>
PublicTransportSystem::parse_groups(const nlohmann::json &raw) {
  std::vector<Group> groups;
  for (const auto &[gid, meta] : raw.at("groups").items())
    groups.push_back({gid, meta.value("name", "")});
  return groups;
}

inline void PublicTransportSystem::init_static_data() {
  if (db_.is_populated())
    throw std::runtime_error(
        "DB already populated. Use force_reinit() to overwrite.");

  auto stops_raw = api_fetch_stops();
  auto services_raw = api_fetch_services_list();

  db_.insert_stops(parse_stops(stops_raw));
  db_.insert_services(parse_services(services_raw));
  db_.insert_groups(parse_groups(services_raw));
}

inline void PublicTransportSystem::force_reinit() {
  db_.exec("DELETE FROM Stops;");
  db_.exec("DELETE FROM Services;");
  db_.exec("DELETE FROM Groups;");
  db_.exec("DELETE FROM ServiceDetails;");
  init_static_data();
}

inline std::vector<Stop>
PublicTransportSystem::search_stops(const std::string &query, int limit) const {
  return db_.search_stops(query, limit);
}

inline std::vector<Service>
PublicTransportSystem::search_services(const std::string &query,
                                       int limit) const {
  return db_.search_services(query, limit);
}

inline std::vector<int> PublicTransportSystem::all_service_ids() const {
  return db_.all_service_ids();
}

inline nlohmann::json PublicTransportSystem::fetch_arrivals(int stop_id) const {
  return api_fetch_stop_arrivals(stop_id);
}

inline nlohmann::json
PublicTransportSystem::fetch_service_detail(int service_id, bool use_cache) {
  if (use_cache) {
    auto cached = db_.get_service_detail(service_id);
    if (cached)
      return *cached;
  }
  auto data = api_fetch_service_detail(service_id);
  db_.upsert_service_detail(service_id, data);
  return data;
}

inline void PublicTransportSystem::fetch_all_service_details(
    std::chrono::milliseconds delay, bool force) {
  for (int sid : db_.all_service_ids()) {
    if (!force && db_.get_service_detail(sid))
      continue;
    auto data = api_fetch_service_detail(sid);
    db_.upsert_service_detail(sid, data);
    std::this_thread::sleep_for(delay);
  }
}

} // namespace mendotran
