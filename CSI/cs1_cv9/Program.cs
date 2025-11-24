using System.IO.Compression;
using System.Text;

namespace cs1_cv9
{
    internal class Program
    {
        static void Main(string[] args)
        {
            int[] nums = [1, 7, 2, 99, 50, 3];
            List<int> nums2 = new List<int>(nums);

            Array.Sort(nums, new NumberComparer());
            nums2.Sort();

            Console.WriteLine(string.Join(", ", nums));
            Console.WriteLine(string.Join(", ", nums2));

            List<Contact> contacts = new List<Contact>()
            {
                new Contact() { Name = "Jan", Age = 30, Email = "jan.csharp@vsb.cz", Weight = 85.6, IsAlive = true },
                new Contact() { Name = "Tereza", Age = 89, Email = "tereza.csharp@vsb.cz", Weight = 61, IsAlive = false },
                new Contact() { Name = "Karel", Age = null, Email = "karel.csharp@vsb.cz", Weight = 102.5, IsAlive = true },
                new Contact() { Name = "Tomáš", Age = 14, Email = "tomas.csharp@vsb.cz", Weight = 45, IsAlive = true },
            };

            contacts.Sort(new ContactComparer());

            foreach (Contact contact in contacts)
            {
                Console.WriteLine(contact.Name);
            }

            //TextFile(contacts);
            //BinaryFile(contacts);
            //MemoryFile(contacts);
            NumberFile(nums2);
        }

        private static void NumberFile(List<int> nums)
        {
            using (FileStream fs = new FileStream("output.bin", FileMode.Create))
            {
                using (BinaryWriter bw = new BinaryWriter(fs, Encoding.UTF8, true))
                {
                    bw.Write(nums.Count);

                    foreach (int num in nums)
                    {
                        bw.Write(num);
                    }
                }

                fs.Seek(0, SeekOrigin.Begin);

                using BinaryReader br = new BinaryReader(fs);

                int count = br.ReadInt32();

                for (int i = 0; i < count; i++)
                {
                    int num = br.ReadInt32();
                    Console.WriteLine(num);
                }
            }
        }

        private static void MemoryFile(List<Contact> contacts)
        {
            using (MemoryStream fs = new MemoryStream())
            {
                using (BinaryWriter bw = new BinaryWriter(fs, Encoding.UTF8, true))
                {

                    bw.Write(contacts.Count);

                    foreach (Contact contact in contacts)
                    {
                        bw.Write(contact.Name);
                        bw.Write(contact.Age.HasValue);
                        if (contact.Age.HasValue)
                        {
                            bw.Write(contact.Age.Value);
                        }
                        bw.Write(contact.Email);
                        bw.Write(contact.Weight);
                        bw.Write(contact.IsAlive);
                    }
                }

                fs.Seek(0, SeekOrigin.Begin);

                using BinaryReader br = new BinaryReader(fs);
                int count = br.ReadInt32();

                for (int i = 0; i < count; i++)
                {
                    string name = br.ReadString();
                    int? age = br.ReadBoolean() ? br.ReadInt32() : null;
                    string email = br.ReadString();
                    double weight = br.ReadDouble();
                    bool isAlive = br.ReadBoolean();

                    Console.WriteLine(name + ": " + age);
                }
            }
        }

        private static void BinaryFile(List<Contact> contacts)
        {
            using (FileStream fs = new FileStream("data.bin", FileMode.Create))
            {
                using (BinaryWriter bw = new BinaryWriter(fs, Encoding.UTF8, true))
                {

                    bw.Write(contacts.Count);

                    foreach (Contact contact in contacts)
                    {
                        bw.Write(contact.Name);
                        bw.Write(contact.Age.HasValue);
                        if (contact.Age.HasValue)
                        {
                            bw.Write(contact.Age.Value);
                        }
                        bw.Write(contact.Email);
                        bw.Write(contact.Weight);
                        bw.Write(contact.IsAlive);
                    }
                }

                fs.Seek(0, SeekOrigin.Begin);

                using BinaryReader br = new BinaryReader(fs);
                int count = br.ReadInt32();

                for (int i = 0; i < count; i++)
                {
                    string name = br.ReadString();
                    int? age = br.ReadBoolean() ? br.ReadInt32() : null;
                    string email = br.ReadString();
                    double weight = br.ReadDouble();
                    bool isAlive = br.ReadBoolean();

                    Console.WriteLine(name + ": " + age);
                }
            }
        }

        private static void TextFile(List<Contact> contacts)
        {
            string txt = "Ahoj! !@#+ěš";

            //byte[] data = Encoding.UTF8.GetBytes(txt);

            //FileStream fs = new FileStream("data.txt", FileMode.Create);

            //try
            //{
            //    fs.Write(data);
            //}
            //finally
            //{
            //    fs.Close();
            //}

            using (FileStream fs = new FileStream("data.txt", FileMode.Create))
            {
                using GZipStream gz = new GZipStream(fs, CompressionLevel.Optimal);

                using (StreamWriter sw = new StreamWriter(gz, Encoding.UTF8))
                {
                    foreach (Contact c in contacts)
                    {
                        sw.Write(c.Name);
                        sw.Write(";");
                        sw.Write(c.Age);
                        sw.Write(";");
                        sw.Write(c.Email);
                        sw.Write(";");
                        sw.Write(c.Weight);
                        sw.Write(";");
                        sw.Write(c.IsAlive);
                        sw.WriteLine();
                    }
                }
            }

            using (FileStream fs2 = new FileStream("data.txt", FileMode.Open))
            {
                using GZipStream gz2 = new GZipStream(fs2, CompressionMode.Decompress);

                using (StreamReader sr = new StreamReader(gz2))
                {
                    while (true)
                    {
                        string line = sr.ReadLine();
                        if (line == null)
                        {
                            break;
                        }

                        Console.WriteLine(line);
                    }
                }
            }
        }
    }
}
