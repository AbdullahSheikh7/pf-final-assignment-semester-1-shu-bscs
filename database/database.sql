CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT UNIQUE NOT NULL,
  password TEXT NOT NULL,
  created_at TEXT DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS resources (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  category TEXT NOT NULL,
  quantity INTEGER NOT NULL CHECK(quantity >= 0),
  unit TEXT NOT NULL,
  status TEXT NOT NULL CHECK(
    status IN (
      'Available',
      'Dispatched',
      'Low Stock',
      'Out of Stock'
    )
  ),
  is_returnable INTEGER NOT NULL CHECK(is_returnable IN (0, 1)),
  issued_count INTEGER DEFAULT 0,
  returned_count INTEGER DEFAULT 0,
  return_status TEXT CHECK(
    return_status IN ('Issued', 'Returned', 'Damaged', NULL)
  ),
  last_updated_by INTEGER,
  last_updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY(last_updated_by) REFERENCES users(id)
);
CREATE TABLE IF NOT EXISTS disaster_zones (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  location TEXT NOT NULL,
  severity TEXT NOT NULL CHECK(severity IN ('Critical', 'Moderate', 'Safe')),
  status TEXT NOT NULL CHECK(status IN ('Active', 'Resolved')),
  description TEXT,
  created_at TEXT DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE IF NOT EXISTS volunteers (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  user_id INTEGER UNIQUE,
  name TEXT NOT NULL,
  email TEXT,
  phone TEXT,
  skills TEXT,
  status TEXT NOT NULL CHECK(status IN ('Active', 'Unavailable', 'Injured')),
  assigned_zone_id INTEGER,
  created_at TEXT DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY(user_id) REFERENCES users(id),
  FOREIGN KEY(assigned_zone_id) REFERENCES disaster_zones(id)
);
INSERT
  OR IGNORE INTO users (username, password)
VALUES ('admin', 'admin123');