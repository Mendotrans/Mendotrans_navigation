#include "transit_db.h"
#include <stdexcept>

namespace mendotran {

inline TransitDatabase::TransitDatabase(const std::string &db_path) {
  int rc = sqlite3_open(db_path.c_str(), &db_);
  if (rc != SQLITE_OK)
    throw std::runtime_error(std::string("sqlite3_open: ") +
                             sqlite3_errmsg(db_));
  exec("PRAGMA journal_mode=WAL;");
  exec("PRAGMA foreign_keys=ON;");
}

inline TransitDatabase::~TransitDatabase() {
  if (db_)
    sqlite3_close(db_);
}

inline void TransitDatabase::check(int rc, const char *context) const {
  if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE)
    throw std::runtime_error(std::string(context) + ": " + sqlite3_errmsg(db_));
}

inline void TransitDatabase::exec(const char *sql) const {
  char *errmsg = nullptr;
  int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string msg = errmsg ? errmsg : "unknown error";
    sqlite3_free(errmsg);
    throw std::runtime_error(std::string("exec: ") + msg);
  }
}

inline TransitDatabase::Stmt
TransitDatabase::prepare(const std::string &sql) const {
  Stmt s;
  check(sqlite3_prepare_v2(db_, sql.c_str(), -1, &s.ptr, nullptr), "prepare");
  return s;
}

inline void TransitDatabase::init_schema() {
  exec(R"(
        CREATE TABLE IF NOT EXISTS Stops (
            stop_id        INTEGER PRIMARY KEY,
            type           TEXT    NOT NULL,
            coordinate_lat REAL    NOT NULL,
            coordinate_lon REAL    NOT NULL,
            code           TEXT,
            location       TEXT
        );
        CREATE TABLE IF NOT EXISTS Services (
            service_id INTEGER PRIMARY KEY,
            type       TEXT NOT NULL,
            group_id   INTEGER NOT NULL,
            code       TEXT,
            name       TEXT,
            color      TEXT,
            mode       TEXT
        );
        CREATE TABLE IF NOT EXISTS Groups (
            group_id TEXT PRIMARY KEY,
            name     TEXT
        );
        CREATE TABLE IF NOT EXISTS ServiceDetails (
            service_id INTEGER PRIMARY KEY,
            fetched_at TEXT    NOT NULL,
            payload    TEXT    NOT NULL
        );
    )");
}

inline bool TransitDatabase::is_populated() const {
  auto s = prepare("SELECT COUNT(*) FROM Stops;");
  sqlite3_step(s.ptr);
  return sqlite3_column_int(s.ptr, 0) > 0;
}

inline void TransitDatabase::insert_stops(const std::vector<Stop> &stops) {
  exec("BEGIN;");
  auto s = prepare("INSERT OR IGNORE INTO Stops "
                   "(stop_id,type,coordinate_lat,coordinate_lon,code,location)"
                   " VALUES (?,?,?,?,?,?);");
  for (const auto &st : stops) {
    sqlite3_reset(s.ptr);
    sqlite3_bind_int(s.ptr, 1, st.stop_id);
    sqlite3_bind_text(s.ptr, 2, st.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(s.ptr, 3, st.lat);
    sqlite3_bind_double(s.ptr, 4, st.lon);
    sqlite3_bind_text(s.ptr, 5, st.code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.ptr, 6, st.location.c_str(), -1, SQLITE_TRANSIENT);
    check(sqlite3_step(s.ptr), "insert_stops");
  }
  exec("COMMIT;");
}

inline void
TransitDatabase::insert_services(const std::vector<Service> &services) {
  exec("BEGIN;");
  auto s = prepare("INSERT OR IGNORE INTO Services "
                   "(service_id,type,group_id,code,name,color,mode)"
                   " VALUES (?,?,?,?,?,?,?);");
  for (const auto &sv : services) {
    sqlite3_reset(s.ptr);
    sqlite3_bind_int(s.ptr, 1, sv.service_id);
    sqlite3_bind_text(s.ptr, 2, sv.type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.ptr, 3, sv.group_id);
    sqlite3_bind_text(s.ptr, 4, sv.code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.ptr, 5, sv.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.ptr, 6, sv.color.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.ptr, 7, sv.mode.c_str(), -1, SQLITE_TRANSIENT);
    check(sqlite3_step(s.ptr), "insert_services");
  }
  exec("COMMIT;");
}

inline void TransitDatabase::insert_groups(const std::vector<Group> &groups) {
  exec("BEGIN;");
  auto s =
      prepare("INSERT OR IGNORE INTO Groups (group_id,name) VALUES (?,?);");
  for (const auto &g : groups) {
    sqlite3_reset(s.ptr);
    sqlite3_bind_text(s.ptr, 1, g.group_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.ptr, 2, g.name.c_str(), -1, SQLITE_TRANSIENT);
    check(sqlite3_step(s.ptr), "insert_groups");
  }
  exec("COMMIT;");
}

inline void
TransitDatabase::upsert_service_detail(int service_id,
                                       const nlohmann::json &payload) {
  // Capture wall-clock time as ISO-8601.
  time_t now = time(nullptr);
  char ts[32];
  strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", localtime(&now));

  auto s = prepare(
      "INSERT OR REPLACE INTO ServiceDetails (service_id,fetched_at,payload)"
      " VALUES (?,?,?);");
  sqlite3_bind_int(s.ptr, 1, service_id);
  sqlite3_bind_text(s.ptr, 2, ts, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(s.ptr, 3, payload.dump().c_str(), -1, SQLITE_TRANSIENT);
  check(sqlite3_step(s.ptr), "upsert_service_detail");
}

inline std::vector<Stop> TransitDatabase::search_stops(const std::string &query,
                                                       int limit) const {
  std::string pat = "%" + query + "%";
  auto s =
      prepare("SELECT stop_id,type,coordinate_lat,coordinate_lon,code,location"
              " FROM Stops WHERE location LIKE ? OR code LIKE ? LIMIT ?;");
  sqlite3_bind_text(s.ptr, 1, pat.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(s.ptr, 2, pat.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(s.ptr, 3, limit);

  std::vector<Stop> result;
  while (sqlite3_step(s.ptr) == SQLITE_ROW) {
    Stop st;
    st.stop_id = sqlite3_column_int(s.ptr, 0);
    st.type = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 1));
    st.lat = sqlite3_column_double(s.ptr, 2);
    st.lon = sqlite3_column_double(s.ptr, 3);
    st.code = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 4));
    st.location = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 5));
    result.push_back(std::move(st));
  }
  return result;
}

inline std::vector<Service>
TransitDatabase::search_services(const std::string &query, int limit) const {
  std::string pat = "%" + query + "%";
  auto s = prepare("SELECT service_id,type,group_id,code,name,color,mode"
                   " FROM Services WHERE name LIKE ? OR code LIKE ? LIMIT ?;");
  sqlite3_bind_text(s.ptr, 1, pat.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(s.ptr, 2, pat.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(s.ptr, 3, limit);

  std::vector<Service> result;
  while (sqlite3_step(s.ptr) == SQLITE_ROW) {
    Service sv;
    sv.service_id = sqlite3_column_int(s.ptr, 0);
    sv.type = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 1));
    sv.group_id = sqlite3_column_int(s.ptr, 2);
    sv.code = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 3));
    sv.name = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 4));
    sv.color = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 5));
    sv.mode = reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 6));
    result.push_back(std::move(sv));
  }
  return result;
}

inline std::vector<int> TransitDatabase::all_service_ids() const {
  auto s = prepare("SELECT service_id FROM Services;");
  std::vector<int> ids;
  while (sqlite3_step(s.ptr) == SQLITE_ROW)
    ids.push_back(sqlite3_column_int(s.ptr, 0));
  return ids;
}

inline std::optional<nlohmann::json>
TransitDatabase::get_service_detail(int service_id) const {
  auto s = prepare("SELECT payload FROM ServiceDetails WHERE service_id=?;");
  sqlite3_bind_int(s.ptr, 1, service_id);
  if (sqlite3_step(s.ptr) != SQLITE_ROW)
    return std::nullopt;
  const char *text =
      reinterpret_cast<const char *>(sqlite3_column_text(s.ptr, 0));
  return nlohmann::json::parse(text);
}

} // namespace mendotran
