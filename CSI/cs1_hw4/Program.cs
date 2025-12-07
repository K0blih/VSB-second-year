using System.ComponentModel.DataAnnotations;
using System.Text;

namespace cs1_hw4
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = Encoding.UTF8;
            Console.InputEncoding = Encoding.UTF8;

            ObservableList<Customer> list = new ObservableList<Customer>();
            list.OnAdd += HandleAdd;
            list.OnRemove += HandleRemove;


            LoadFromBin(list, args[0]);

            list.Remove(list[2]);

            string json = list.SerializeToJson();
            string xml = list.SerializeToXml();

            // Tento řádek je nutný kvůli správnému fungování v Kelvinu!
            xml = xml.Trim(new char[] { '\uFEFF', '\u200B' });

            SaveToTextFile(json, "customers.json");
            SaveToTextFile(xml, "customers.xml");

            Console.WriteLine(new string('\n', 5));

            Console.WriteLine(new string('=', 20));
            Console.WriteLine("JSON data");
            Console.WriteLine(new string('=', 20));
            Console.WriteLine();

            ObservableList<Customer> jsonCustomers = ObservableList<Customer>.DeserializeFromJson(
                LoadFromTextFile("customers.json")
                );
            Print(jsonCustomers);

            Console.WriteLine(new string('\n', 5));

            Console.WriteLine(new string('=', 20));
            Console.WriteLine("XML data");
            Console.WriteLine(new string('=', 20));
            Console.WriteLine();

            ObservableList<Customer> xmlCustomers = ObservableList<Customer>.DeserializeFromXml(
                LoadFromTextFile("customers.xml")
                );
            Print(xmlCustomers);
        }

        private static void HandleAdd(Customer c)
        {
            Console.WriteLine($"Přidáno: {c.Name} ({c.Age})");
        }

        private static void HandleRemove(Customer c)
        {
            Console.WriteLine($"Odebráno: {c.Name} ({c.Age})");
        }

        private static void LoadFromBin(ObservableList<Customer> list, string filePath)
        {
            using FileStream fs = new FileStream(filePath, FileMode.Open);
            using BinaryReader br = new BinaryReader(fs);
            int length = br.ReadInt32();

            for(int i = 0; i < length; i++)
            {
                string name = br.ReadString();
                int age = br.ReadInt32();
                list.Add(new Customer { Name = name, Age = age});
            }
        }

        public static void SaveToTextFile(string content, string fileName)
        {
            File.WriteAllText(fileName, content);
        }

        public static string LoadFromTextFile(string fileName)
        {
            return File.ReadAllText(fileName);
        }

        private static void Print(ObservableList<Customer> customers)
        {
            Console.WriteLine("Zákazníci:");
            Console.WriteLine(new string('-', 15));
            foreach (Customer num in customers)
            {
                Console.WriteLine(num);
            }


            Console.WriteLine("\n");
            Console.WriteLine("Nezletilí zákazníci:");
            Console.WriteLine(new string('-', 15));
            foreach (Customer num in customers.Filter(
                    c => c.Age < 18
            ))
            {
                Console.WriteLine(num);
            }


            Console.WriteLine("\n");
            Console.WriteLine("Zákazníci seřazení podle věku:");
            Console.WriteLine(new string('-', 15));
            foreach (Customer num in customers.OrderBy(new AgeComparer()))
            {
                Console.WriteLine(num);
            }
        }
    }
}
