PRAGMA foreign_keys = ON;
CREATE TABLE IF NOT EXISTS Users (
  user_id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL UNIQUE,
  password_hash TEXT NOT NULL,
  -- store hashes (see README)
  role TEXT NOT NULL DEFAULT 'coordinator',
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS DisasterZones (
  zone_id INTEGER PRIMARY KEY AUTOINCREMENT,
  zone_name TEXT NOT NULL,
  location TEXT,
  severity_level TEXT,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS Resources (
  resource_id INTEGER PRIMARY KEY AUTOINCREMENT,
  resource_name TEXT NOT NULL,
  quantity INTEGER DEFAULT 0,
  location TEXT,
  added_by INTEGER,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (added_by) REFERENCES Users(user_id) ON DELETE
  SET NULL
);
CREATE TABLE IF NOT EXISTS Volunteers (
  volunteer_id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  contact_info TEXT,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS VolunteerAssignments (
  assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
  volunteer_id INTEGER NOT NULL,
  zone_id INTEGER NOT NULL,
  role TEXT,
  assigned_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (volunteer_id) REFERENCES Volunteers(volunteer_id) ON DELETE CASCADE,
  FOREIGN KEY (zone_id) REFERENCES DisasterZones(zone_id) ON DELETE CASCADE,
  UNIQUE(volunteer_id, zone_id)
);
CREATE TABLE IF NOT EXISTS Requests (
  request_id INTEGER PRIMARY KEY AUTOINCREMENT,
  zone_id INTEGER NOT NULL,
  resource_id INTEGER NOT NULL,
  quantity_needed INTEGER NOT NULL,
  status TEXT DEFAULT 'Pending',
  requested_by INTEGER,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (zone_id) REFERENCES DisasterZones(zone_id) ON DELETE CASCADE,
  FOREIGN KEY (resource_id) REFERENCES Resources(resource_id) ON DELETE RESTRICT,
  FOREIGN KEY (requested_by) REFERENCES Users(user_id) ON DELETE
  SET NULL
);
-- Optional: simple Reports table
CREATE TABLE IF NOT EXISTS Reports (
  report_id INTEGER PRIMARY KEY AUTOINCREMENT,
  report_type TEXT,
  generated_by INTEGER,
  content TEXT,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (generated_by) REFERENCES Users(user_id) ON DELETE
  SET NULL
);