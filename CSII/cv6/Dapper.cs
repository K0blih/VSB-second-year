using Dapper;
using Microsoft.Data.Sqlite;
using System.Net;
using System.Xml.Linq;

namespace cv6
{
    // TODO: „Microsoft.EntityFrameworkCore.Sqlite“ a "Microsoft.EntityFrameworkCore.Tools" mirror this file
    internal class Dapper
    {
        class Customer
        {
            public int Id { get; set; }
            [Column("Name")] // nazev sloupce v db
            public string Name { get; set; }
            public string Address { get; set; }
        }

        static void Main(string[] args)c
        {
            string createSQL = """
                CREATE TABLE "Customer" (
                	"Id"	INTEGER,
                	"Name"	TEXT NOT NULL,
                	"Address"	TEXT,
                	PRIMARY KEY("Id" AUTOINCREMENT)
                );

                CREATE TABLE "Order" (
                	"Id"	INTEGER,
                	"CustomerId"	INTEGER NOT NULL,
                	"Product"	TEXT NOT NULL,
                	"Price"	NUMERIC NOT NULL,
                	FOREIGN KEY("CustomerId") REFERENCES "Customer"("Id"),
                	PRIMARY KEY("Id" AUTOINCREMENT)
                );
                """;

            string connectionString = "Data Source=mydb.db;";

            SimpleCRUD.SetDialect(SimpleCRUD.Dialect.SQLite);

            using (SqliteConnection connection = new SqliteConnection(connectionString))
            {
                connection.Open();

                //using SqliteCommand createCmd = new SqliteCommand();
                //createCmd.CommandText = createSQL;
                //createCmd.Connection = connection;
                //createCmd.ExecuteNonQuery();

                using SqliteTransaction transaction = connection.BeginTransaction();

                string customerName = "Tomas";

                int? id = connection.Insert(new Customer()
                {
                    Name = "Katerina",
                    Address = "Plzen"
                }, transaction);

                Customer customer = connection.Get<Customer>(id.Value);
                customer.Address = "Nova ves";
                connection.Update(customer);

                //connection.Execute(
                //    "INSERT INTO Customer (Name, Address) VALUES (@Jmeno, @Adresa)",
                //    new { Jmeno = "Zuzana", Adresa = (string)null },
                //    transaction
                //    );

                //using SqliteCommand insertCmd = new SqliteCommand(
                //    "INSERT INTO Customer (Name, Address) VALUES (@Jmeno, @Adresa)",
                //    connection,
                //    transaction
                //    );
                //insertCmd.Parameters.AddWithValue("Jmeno", customerName);
                //insertCmd.Parameters.AddWithValue("Adresa", DBNull.Value);
                //insertCmd.Parameters.Add(new SqliteParameter()
                //{
                //    ParameterName = "Adresa",
                //    Value = DBNull.Value,
                //    SqliteType = SqliteType.Text
                //    //Direction = System.Data.ParameterDirection.Output
                //});
                //insertCmd.ExecuteNonQuery();

                //--

                foreach (Customer c in connection.GetList<Customer>())
                //foreach (Customer c in connection.Query<Customer>("SELECT * FROM Customer", null, transaction))
                {
                    Console.WriteLine($"{c.Id} | {c.Name} | {c.Address}");
                }

                //using SqliteCommand selectCmd = new SqliteCommand(
                //    "SELECT * FROM Customer",
                //    connection,
                //    transaction
                //    );
                //using SqliteDataReader reader = selectCmd.ExecuteReader();
                //while (reader.Read())
                //{
                //    int id = reader.GetInt32(reader.GetOrdinal("Id"));
                //    string name = reader.GetString(reader.GetOrdinal("Name"));
                //    string address = null;
                //    if (!reader.IsDBNull(reader.GetOrdinal("Address")))
                //    {
                //        address = reader.GetString(reader.GetOrdinal("Address"));
                //    }

                //    Console.WriteLine($"{id} | {name} | {address}");
                //}

                long count = connection.RecordCount<Customer>();
                //long count = connection.ExecuteScalar<long>("SELECT COUNT(*) FROM Customer", null, transaction);

                //using SqliteCommand countCmd = new SqliteCommand(
                //    "SELECT COUNT(*) FROM Customer",
                //    connection,
                //    transaction
                //    );
                //long count = (long)countCmd.ExecuteScalar();
                Console.WriteLine("Pocet: " + count);

                transaction.Commit();

                //transaction.Rollback();

                connection.Close();
            }
        }
    }
}
