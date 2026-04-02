using Dapper;
using Microsoft.Data.Sqlite;
using Microsoft.EntityFrameworkCore;

namespace ConsoleApp3
{
    internal class Program
    {
        static void Main(string[] args)
        {
           
           /* string createSql = """
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
            string connectionString = "Data Source=mydb.db";
            using SqliteConnection connection = new SqliteConnection(connectionString);

            connection.Open();


            
            using SqliteCommand command = new SqliteCommand();

            command.CommandText = createSql;
            command.Connection = connection;
            command.ExecuteNonQuery();

            SimpleCRUD.SetDialect(SimpleCRUD.Dialect.SQLite);

            using SqliteTransaction transaction = connection.BeginTransaction();

            int? id = connection.Insert(new Customer()
            {
                Name = "Kateřina",
                Address = "Nová Ves"
            }, transaction);

            Customer cus = connection.Get<Customer>(id);
            cus.Address = "Zlín";
            connection.Update(cus);


           using SqliteCommand command1 = new SqliteCommand(
                "INSERT INTO Customer (Name, Address) VALUES (@Jmeno, @Adresa)",
                connection, transaction
                );

            command1.Parameters.AddWithValue("Jmeno", "Zuzana");
            command1.Parameters.Add(new SqliteParameter()
            {
                ParameterName = "Address",
                Value = DBNull.Value,
                DbType = System.Data.DbType.String
            });

            command1.ExecuteNonQuery();
           
            
            foreach(Customer c in connection.Query<Customer>("SELECT * FROM Customer", null, transaction))
            {
                Console.WriteLine($"{c.Id} | {c.Name} | {c.Address}");

            }

            using SqliteCommand command = new SqliteCommand(
                    "SELECT * FROM Customer",
                    connection, transaction
                );

            using SqliteDataReader reader = command.ExecuteReader();

            while (reader.Read())
            {
                int id = reader.GetInt32(reader.GetOrdinal("Id"));
                string name = reader.GetString(reader.GetOrdinal("Name"));
                string address = null;

                if(!reader.IsDBNull(reader.GetOrdinal("Address")))
                {
                   address = reader.GetString(reader.GetOrdinal("Address"));
                }
                
                Console.WriteLine(id + " | " + name + " | " + address);
            }

            long count = connection.ExecuteScalar<long>("SELECT COUNT(*) FROM CUSTOMER", null, transaction);

            Console.WriteLine("Počet: " + count);
            transaction.Commit();

            using SqliteCommand countCmd = new SqliteCommand(
                "SELECT COUNT(*) FROM CUSTOMER",
                connection
                );
            long count = (long)countCmd.ExecuteScalar();
            
            */


            MyDb db = new MyDb();
            db.Customer.Add(new Customer()
            {
                Name = "Jana",
                Address = "Klimkovice"
            });

            db.SaveChangesAsync();

            foreach (Customer customer in db.Customer.Where(x => x.Id < 5).OrderByDescending(x => x.Name))
            {
            
                Console.WriteLine($"{customer.Id} | {customer.Name} | {customer.Address}");
            }

            db.Order.Add(new Order()
            {
                Price = 2000,
                Product = "Banány",
                Customer = new Customer()
                {
                    Name = "Karel",
                    Address = "Nová Ves"
                }
            });

            db.SaveChangesAsync();

            foreach(Order o in db.Order.Include(x => x.Customer))
            {
                Console.WriteLine(o.Product + " - " + o.Customer.Name);
            }

        }
    }
}
