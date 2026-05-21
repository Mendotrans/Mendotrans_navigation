#pragma once

#include "mendotran_types.h"

#include <nlohmann/json.hpp>

#include <optional>
#include <sqlite3.h>

#include <string>
#include <vector>

namespace mendotran {

// Wraps all SQLite interactions. Owns the database connection lifetime.
class TransitDatabase {
public:
  explicit TransitDatabase(const std::string &db_path);
  ~TransitDatabase();

  TransitDatabase(const TransitDatabase &) = delete;
  TransitDatabase &operator=(const TransitDatabase &) = delete;

  // Schema creation — idempotent, safe to call on existing DB.
  void init_schema();

  // Returns true if the Stops table already has rows.
  bool is_populated() const;

  // Clears the Stops, Services, Groups an ServiceDetails tables
  void clear_data();

  // Bulk insert helpers called during the initial static data load.
  void insert_stops(const std::vector<Stop> &stops);
  void insert_services(const std::vector<Service> &services);
  void insert_groups(const std::vector<Group> &groups);

  // Upserts a JSON blob for a service's detail payload.
  void upsert_service_detail(int service_id, const nlohmann::json &payload);

  // Lookups.
  std::vector<Stop> search_stops(const std::string &query,
                                 int limit = 20) const;

  std::vector<Stop> get_all_stops() const;

  std::vector<Service> search_services(const std::string &query,
                                       int limit = 30) const;
  std::vector<int> all_service_ids() const;

  // Returns nullopt if no cached detail exists for service_id.
  std::optional<nlohmann::json> get_service_detail(int service_id) const;

private:
  sqlite3 *db_ = nullptr;

  // Thin RAII helper around sqlite3_stmt.
  struct Stmt {
    sqlite3_stmt *ptr = nullptr;
    ~Stmt() {
      if (ptr)
        sqlite3_finalize(ptr);
    }
  };

  void exec(const char *sql) const;
  Stmt prepare(const std::string &sql) const;
  void check(int rc, const char *context = "") const;
};

} // namespace mendotran
