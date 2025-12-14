#include <iostream>
#include <sqlite3.h>
#include <iomanip>

using namespace std;

sqlite3 *db = nullptr;
string dbPath = "database/database.db";
int currentUserId = -1;
string currentUsername = "";

void pauseScreen()
{
  cout << "\nPress Enter to continue...";
  cin.ignore(1000, '\n');
  cin.get();
}

void splashScreen()
{
  string titleLines[9] = {
      "         ____  _               _              ____      _ _       __",
      "        |  _ \\(_)___  __ _ ___| |_ ___ _ __  |  _ \\ ___| (_) ___ / _|",
      "        | | | | / __|/ _` / __| __/ _ \\ '__| | |_) / _ \\ | |/ _ \\ |_",
      "        | |_| | \\__ \\ (_| \\__ \\ ||  __/ |    |  _ <  __/ | |  __/  _|",
      "  ____  |____/|_|___/\\__,_|___/\\__\\___|_|    |_| \\_\\___|_|_|\\___|_|",
      " |  _ \\ ___  ___  ___  _   _ _ __ ___ ___  |_   _| __ __ _  ___| | _____ _ __",
      " | |_) / _ \\/ __|/ _ \\| | | | '__/ __/ _ \\   | || '__/ _` |/ __| |/ / _ \\ '__|",
      " |  _ <  __/\\__ \\ (_) | |_| | | | (_|  __/   | || | | (_| | (__|   <  __/ |",
      " |_| \\_\\___||___/\\___/ \\__,_|_|  \\___\\___|   |_||_|  \\__,_|\\___|_|\\_\\___|_|"};

  for (int i = 0; i < 9; i++)
    cout << titleLines[i] << endl;

  pauseScreen();
}

int openDatabase(const string &db_filename)
{
  int exit = sqlite3_open(db_filename.c_str(), &db);

  if (exit != SQLITE_OK)
  {
    cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
    if (db)
    {
      sqlite3_close(db);
    }
    return 0;
  }
  else
  {
    cout << "Successfully opened/created database: " << db_filename << endl;
    return 1;
  }
}

void closeDatabase()
{
  if (db)
  {
    sqlite3_close(db);
    cout << "Database connection closed." << endl;
  }
}

bool executeSQL(const string &sql)
{
  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK)
  {
    cout << "SQL error: " << errMsg << endl;
    sqlite3_free(errMsg);
    return false;
  }
  return true;
}

string getCurrentTimestamp()
{
  time_t now = time(0);
  tm *ltm = localtime(&now);
  stringstream ss;
  ss << (1900 + ltm->tm_year) << "-"
     << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-"
     << setfill('0') << setw(2) << ltm->tm_mday << " "
     << setfill('0') << setw(2) << ltm->tm_hour << ":"
     << setfill('0') << setw(2) << ltm->tm_min << ":"
     << setfill('0') << setw(2) << ltm->tm_sec;
  return ss.str();
}

void clearScreen()
{
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

bool loginUser()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "           LOGIN SYSTEM                 " << endl;
  cout << "========================================" << endl;

  string username, password;
  cout << "Enter username: ";
  cin >> username;
  cout << "Enter password: ";
  cin >> password;

  sqlite3_stmt *stmt;
  string sql = "SELECT id, username FROM users WHERE username = ? AND password = ?";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement!" << endl;
    cout << "Error Code: " << rc << endl;
    cout << "Error Message: " << sqlite3_errmsg(db) << endl;
    pauseScreen();
    return false;
  }

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW)
  {
    currentUserId = sqlite3_column_int(stmt, 0);
    currentUsername = string((const char *)sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    cout << "\nLogin successful! Welcome " << currentUsername << endl;
    return true;
  }

  sqlite3_finalize(stmt);
  cout << "\nInvalid username or password!" << endl;
  pauseScreen();
  return false;
}

void viewResources()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "          VIEW RESOURCES                " << endl;
  cout << "========================================" << endl;

  sqlite3_stmt *stmt;
  string sql = "SELECT id, name, category, quantity, unit, status, is_returnable, issued_count, returned_count FROM resources ORDER BY id";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to retrieve resources" << endl;
    pauseScreen();
    return;
  }

  cout << endl
       << left << setw(3) << "ID" << " | " << setw(10) << "Name" << " | " << setw(10) << "Category" << " | " << setw(10) << "Quantity" << " | " << setw(10) << "Unit" << " | " << setw(10) << "Status" << " | " << setw(10) << "Returnable" << " | " << setw(10) << "Issued" << " | " << setw(10) << "Returned" << endl;
  cout << "---------------------------------------------------------------------------------------------------------" << endl;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    cout << setw(3) << sqlite3_column_int(stmt, 0) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 1) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 2) << " | ";
    cout << setw(10) << sqlite3_column_int(stmt, 3) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 4) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 5) << " | ";
    string isR = sqlite3_column_int(stmt, 6) ? "Yes" : "No";
    cout << setw(10) << isR << " | ";
    cout << setw(10) << sqlite3_column_int(stmt, 7) << " | ";
    cout << setw(10) << sqlite3_column_int(stmt, 8) << endl;
  }

  sqlite3_finalize(stmt);
  pauseScreen();
}

void addResource()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "          ADD RESOURCE                  " << endl;
  cout << "========================================" << endl;

  string name, category, unit, status;
  int quantity, isReturnable;

  cout << "Enter resource name: ";
  cin.ignore();
  getline(cin, name);
  cout << "Enter category: ";
  getline(cin, category);

  do
  {
    cout << "Enter quantity: ";
    cin >> quantity;
    if (quantity < 0)
    {
      cout << "Invalid input. Quantity must be >= 0" << endl;
    }
  } while (quantity < 0);

  cout << "Enter unit: ";
  cin.ignore();
  getline(cin, unit);

  if (quantity == 0)
  {
    status = "Out of Stock";
  }
  else if (quantity < 10)
  {
    status = "Low Stock";
  }
  else
  {
    status = "Available";
  }

  do
  {
    cout << "Is returnable? (1=Yes, 0=No): ";
    cin >> isReturnable;
    if (isReturnable != 0 && isReturnable != 1)
    {
      cout << "Invalid input. Enter 0 or 1." << endl;
    }
  } while (isReturnable != 0 && isReturnable != 1);

  sqlite3_stmt *stmt;
  string sql = "INSERT INTO resources (name, category, quantity, unit, status, is_returnable, last_updated_by, last_updated_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement" << endl;
    pauseScreen();
    return;
  }

  string timestamp = getCurrentTimestamp();
  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, quantity);
  sqlite3_bind_text(stmt, 4, unit.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, status.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 6, isReturnable);
  sqlite3_bind_int(stmt, 7, currentUserId);
  sqlite3_bind_text(stmt, 8, timestamp.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc == SQLITE_DONE)
  {
    cout << "\nResource added successfully!" << endl;
  }
  else
  {
    cout << "\nFailed to add resource!" << endl;
  }
  pauseScreen();
}

void updateResource()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "         UPDATE RESOURCE                " << endl;
  cout << "========================================" << endl;

  int resourceId, newQuantity;

  do
  {
    cout << "Enter resource ID to update: ";
    cin >> resourceId;
    if (resourceId < 1)
    {
      cout << "Invalid ID. Must be >= 1" << endl;
    }
  } while (resourceId < 1);

  do
  {
    cout << "Enter new quantity: ";
    cin >> newQuantity;
    if (newQuantity < 0)
    {
      cout << "Invalid input. Quantity must be >= 0" << endl;
    }
  } while (newQuantity < 0);

  string status;
  if (newQuantity == 0)
  {
    status = "Out of Stock";
  }
  else if (newQuantity < 10)
  {
    status = "Low Stock";
  }
  else
  {
    status = "Available";
  }

  sqlite3_stmt *stmt;
  string sql = "UPDATE resources SET quantity = ?, status = ?, last_updated_by = ?, last_updated_at = ? WHERE id = ?";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement" << endl;
    pauseScreen();
    return;
  }

  string timestamp = getCurrentTimestamp();
  sqlite3_bind_int(stmt, 1, newQuantity);
  sqlite3_bind_text(stmt, 2, status.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, currentUserId);
  sqlite3_bind_text(stmt, 4, timestamp.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 5, resourceId);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc == SQLITE_DONE)
  {
    cout << "\nResource updated successfully!" << endl;
  }
  else
  {
    cout << "\nFailed to update resource!" << endl;
  }
  pauseScreen();
}

void viewDisasterZones()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "        VIEW DISASTER ZONES             " << endl;
  cout << "========================================" << endl;

  sqlite3_stmt *stmt;
  string sql = "SELECT id, name, location, severity, status, description FROM disaster_zones ORDER BY id";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to retrieve zones" << endl;
    pauseScreen();
    return;
  }

  cout << endl
       << left << setw(3) << "ID" << " | " << setw(20) << "Name" << " | " << setw(20) << "Location" << " | " << setw(10) << "Severity" << " | " << setw(10) << "Status" << " | Description " << endl;
  cout << "---------------------------------------------------------------------------------------------------------" << endl;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    cout << setw(3) << sqlite3_column_int(stmt, 0) << " | ";
    cout << setw(20) << sqlite3_column_text(stmt, 1) << " | ";
    cout << setw(20) << sqlite3_column_text(stmt, 2) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 3) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 4) << " | ";
    cout << sqlite3_column_text(stmt, 5) << endl;
  }

  sqlite3_finalize(stmt);
  pauseScreen();
}

void addDisasterZone()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "         ADD DISASTER ZONE              " << endl;
  cout << "========================================" << endl;

  string name, location, severity, description;
  int severityChoice;

  cout << "Enter zone name: ";
  cin.ignore();
  getline(cin, name);

  cout << "Enter location: ";
  getline(cin, location);

  do
  {
    cout << "Select severity:\n1. Critical\n2. Moderate\n3. Safe\nChoice: ";
    cin >> severityChoice;
    if (severityChoice < 1 || severityChoice > 3)
    {
      cout << "Invalid choice." << endl;
    }
  } while (severityChoice < 1 || severityChoice > 3);

  if (severityChoice == 1)
    severity = "Critical";
  else if (severityChoice == 2)
    severity = "Moderate";
  else
    severity = "Safe";

  cout << "Enter description: ";
  cin.ignore();
  getline(cin, description);

  sqlite3_stmt *stmt;
  string sql = "INSERT INTO disaster_zones (name, location, severity, status, description) VALUES (?, ?, ?, 'Active', ?)";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement" << endl;
    pauseScreen();
    return;
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, location.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc == SQLITE_DONE)
  {
    cout << "\nDisaster zone added successfully!" << endl;
  }
  else
  {
    cout << "\nFailed to add zone!" << endl;
  }
  pauseScreen();
}

void markZoneResolved()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "        MARK ZONE RESOLVED              " << endl;
  cout << "========================================" << endl;

  int zoneId;
  do
  {
    cout << "Enter zone ID to mark as resolved: ";
    cin >> zoneId;
    if (zoneId < 1)
    {
      cout << "Invalid ID. Must be >= 1" << endl;
    }
  } while (zoneId < 1);

  string sql = "UPDATE disaster_zones SET status = 'Resolved' WHERE id = " + to_string(zoneId);

  if (executeSQL(sql))
  {
    cout << "\nZone marked as resolved!" << endl;
  }
  else
  {
    cout << "\nFailed to update zone!" << endl;
  }
  pauseScreen();
}

void viewVolunteers()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "         VIEW VOLUNTEERS                " << endl;
  cout << "========================================" << endl;

  sqlite3_stmt *stmt;
  string sql = "SELECT id, name, email, phone, skills, status, assigned_zone_id FROM volunteers ORDER BY id";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to retrieve volunteers" << endl;
    pauseScreen();
    return;
  }

  cout << endl
       << left << setw(3) << "ID" << " | " << setw(10) << "Name" << " | " << setw(20) << "Email" << " | " << setw(10) << "Phone" << " | " << setw(10) << "Skills" << " | " << setw(10) << "Status" << " | " << setw(10) << "Zone" << endl;
  cout << "---------------------------------------------------------------------------------------------------------" << endl;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    cout << setw(3) << sqlite3_column_int(stmt, 0) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 1) << " | ";
    cout << setw(20) << sqlite3_column_text(stmt, 2) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 3) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 4) << " | ";
    cout << setw(10) << sqlite3_column_text(stmt, 5) << " | ";
    if (sqlite3_column_type(stmt, 6) == SQLITE_NULL)
    {
      cout << "NULL" << endl;
    }
    else
    {
      cout << sqlite3_column_int(stmt, 6) << endl;
    }
  }

  sqlite3_finalize(stmt);
  pauseScreen();
}

void addVolunteer()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "          ADD VOLUNTEER                 " << endl;
  cout << "========================================" << endl;

  string name, email, phone, skills, status;
  int statusChoice;

  cout << "Enter volunteer name: ";
  cin.ignore();
  getline(cin, name);
  cout << "Enter email: ";
  getline(cin, email);
  cout << "Enter phone: ";
  getline(cin, phone);
  cout << "Enter skills: ";
  getline(cin, skills);

  do
  {
    cout << "Select status:\n1. Active\n2. Unavailable\n3. Injured\nChoice: ";
    cin >> statusChoice;
    if (statusChoice < 1 || statusChoice > 3)
    {
      cout << "Invalid choice." << endl;
    }
  } while (statusChoice < 1 || statusChoice > 3);

  if (statusChoice == 1)
    status = "Active";
  else if (statusChoice == 2)
    status = "Unavailable";
  else
    status = "Injured";

  sqlite3_stmt *stmt;
  string sql = "INSERT INTO volunteers (name, email, phone, skills, status) VALUES (?, ?, ?, ?, ?)";

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement" << endl;
    pauseScreen();
    return;
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, phone.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, skills.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, status.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc == SQLITE_DONE)
  {
    cout << "\nVolunteer added successfully!" << endl;
  }
  else
  {
    cout << "\nFailed to add volunteer!" << endl;
  }
  pauseScreen();
}

void assignVolunteerToZone()
{
  clearScreen();
  cout << "========================================" << endl;
  cout << "      ASSIGN VOLUNTEER TO ZONE          " << endl;
  cout << "========================================" << endl;

  int volunteerId, zoneId;

  do
  {
    cout << "Enter volunteer ID: ";
    cin >> volunteerId;
    if (volunteerId < 1)
    {
      cout << "Invalid ID. Must be >= 1" << endl;
    }
  } while (volunteerId < 1);

  do
  {
    cout << "Enter zone ID: ";
    cin >> zoneId;
    if (zoneId < 1)
    {
      cout << "Invalid ID. Must be >= 1" << endl;
    }
  } while (zoneId < 1);

  sqlite3_stmt *checkStmt;
  string checkSql = "SELECT status FROM disaster_zones WHERE id = ?";
  int rc = sqlite3_prepare_v2(db, checkSql.c_str(), -1, &checkStmt, nullptr);
  sqlite3_bind_int(checkStmt, 1, zoneId);

  rc = sqlite3_step(checkStmt);
  if (rc == SQLITE_ROW)
  {
    string zoneStatus = string((const char *)sqlite3_column_text(checkStmt, 0));
    sqlite3_finalize(checkStmt);

    if (zoneStatus == "Resolved")
    {
      cout << "\nCannot assign volunteer to resolved zone!" << endl;
      pauseScreen();
      return;
    }
  }
  else
  {
    sqlite3_finalize(checkStmt);
    cout << "\nZone not found!" << endl;
    pauseScreen();
    return;
  }

  sqlite3_stmt *stmt;
  string sql = "UPDATE volunteers SET assigned_zone_id = ? WHERE id = ?";

  rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK)
  {
    cout << "Failed to prepare statement" << endl;
    pauseScreen();
    return;
  }

  sqlite3_bind_int(stmt, 1, zoneId);
  sqlite3_bind_int(stmt, 2, volunteerId);

  rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc == SQLITE_DONE)
  {
    cout << "\nVolunteer assigned successfully!" << endl;
  }
  else
  {
    cout << "\nFailed to assign volunteer!" << endl;
  }
  pauseScreen();
}

void mainDashboard()
{
  int choice;
  do
  {
    clearScreen();
    cout << "========================================" << endl;
    cout << "  DISASTER RELIEF RESOURCE TRACKER      " << endl;
    cout << "========================================" << endl;
    cout << "User: " << currentUsername << endl;
    cout << "========================================" << endl;
    cout << "1. View Resources" << endl;
    cout << "2. Add Resource" << endl;
    cout << "3. Update Resource" << endl;
    cout << "4. View Disaster Zones" << endl;
    cout << "5. Add Disaster Zone" << endl;
    cout << "6. Mark Zone as Resolved" << endl;
    cout << "7. View Volunteers" << endl;
    cout << "8. Add Volunteer" << endl;
    cout << "9. Assign Volunteer to Zone" << endl;
    cout << "10. Logout" << endl;
    cout << "========================================" << endl;

    do
    {
      cout << "Enter choice: ";
      cin >> choice;
      if (choice < 1 || choice > 10)
      {
        cout << "Invalid choice. Enter 1-10." << endl;
      }
    } while (choice < 1 || choice > 10);

    switch (choice)
    {
    case 1:
      viewResources();
      break;
    case 2:
      addResource();
      break;
    case 3:
      updateResource();
      break;
    case 4:
      viewDisasterZones();
      break;
    case 5:
      addDisasterZone();
      break;
    case 6:
      markZoneResolved();
      break;
    case 7:
      viewVolunteers();
      break;
    case 8:
      addVolunteer();
      break;
    case 9:
      assignVolunteerToZone();
      break;
    case 10:
      cout << "\nLogging out..." << endl;
      currentUserId = -1;
      currentUsername = "";
      pauseScreen();
      break;
    }
  } while (choice != 10);
}

int main()
{
  clearScreen();
  splashScreen();

  if (!openDatabase(dbPath))
  {
    cout << "Failed to initialize database" << endl;
    return 1;
  }

  int choice;
  do
  {
    clearScreen();
    cout << "========================================" << endl;
    cout << "  DISASTER RELIEF RESOURCE TRACKER      " << endl;
    cout << "========================================" << endl;
    cout << "1. Login" << endl;
    cout << "2. Exit" << endl;
    cout << "========================================" << endl;

    do
    {
      cout << "Enter choice: ";
      cin >> choice;
      if (choice < 0 && choice > 2)
      {
        cout << "Invalid choice" << endl;
      }
    } while (choice < 0 && choice > 2);

    switch (choice)
    {
    case 1:
      if (loginUser())
      {
        mainDashboard();
      }
      break;
    case 2:
      cout << "\nThank you for using the system!" << endl;
      break;
    default:
      cout << "Invalid choice!" << endl;
      pauseScreen();
    }
  } while (choice != 2);

  closeDatabase();
  return 0;
}
