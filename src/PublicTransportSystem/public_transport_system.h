#pragma once

#include "mendotran_types.h"
#include "transit_db.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <string>
#include <vector>

namespace mendotran {

// Encapsulates all API communication and orchestrates the TransitDatabase.
//
// Static data (stops, services, groups) is downloaded once and stored in the
// DB.  Dynamic data (arrivals, live service detail) is always fetched live.
class PublicTransportSystem {
public:
  // db_path  — filesystem path for the SQLite file.
  // cfg      — all API coordinates and credentials.
  explicit PublicTransportSystem(const std::string &db_path,
                                 const ApiConfig &cfg);

  // Downloads stops, services and groups then populates the DB.
  // Throws if the DB is already populated; call force_reinit() to override.
  void init_static_data();

  // Like init_static_data() but drops existing rows first.
  void force_reinit();

  // ── Static data access (served from DB) ─────────────────────────────

  std::vector<Stop> search_stops(const std::string &query,
                                 int limit = 20) const;
  std::vector<Service> search_services(const std::string &query,
                                       int limit = 30) const;
  std::vector<int> all_service_ids() const;

  // ── Dynamic data (always hits the network) ───────────────────────────

  // Returns the raw arrivals JSON for a stop.
  nlohmann::json fetch_arrivals(int stop_id) const;

  // Fetches (and optionally caches) the detail payload for a service.
  // use_cache=true returns DB data when available; false always re-fetches.
  nlohmann::json fetch_service_detail(int service_id, bool use_cache = true);

  // Bulk-fetches all service details with a configurable inter-request delay.
  // Already-cached services are skipped unless force=true.
  void fetch_all_service_details(
      std::chrono::milliseconds delay = std::chrono::milliseconds(500),
      bool force = false);

  // Direct access to the underlying database.
  TransitDatabase &db() { return db_; }
  const TransitDatabase &db() const { return db_; }

private:
  ApiConfig cfg_;
  TransitDatabase db_;

  // Returns a configured HTTPS or HTTP client for the base URL.
  std::unique_ptr<httplib::Client> make_client() const;

  // Common POST helper.  endpoint is relative to cfg_.base_path.
  nlohmann::json post(const std::string &endpoint,
                      const nlohmann::json &body) const;

  // Individual static-fetch calls.
  nlohmann::json api_fetch_stops() const;
  nlohmann::json api_fetch_services_list() const;

  // Individual dynamic-fetch calls.
  nlohmann::json api_fetch_stop_arrivals(int stop_id) const;
  nlohmann::json api_fetch_service_detail(int service_id) const;

  // Parses raw search JSON into domain structs.
  static std::vector<Stop> parse_stops(const nlohmann::json &raw);
  static std::vector<Service> parse_services(const nlohmann::json &raw);
  static std::vector<Group> parse_groups(const nlohmann::json &raw);
};

} // namespace mendotran
