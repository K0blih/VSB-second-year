using Database;

namespace cs1_cv4
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Person p = new Person()
            {
                Name = "Jan",
                Age = 18,
                Gender = GenderEnum.MALE
            };

            if (p.IsAdult)
            {
                Console.WriteLine("Is adult.");
            }

            Console.WriteLine(p);

            PopulationDatabase pop = new PopulationDatabase();
            pop.Add(p);
            pop.Add(p);
            pop.Add(p);
            pop.Add(p);
            pop.Add(p);

            Console.WriteLine(pop.Count);
            Console.WriteLine(pop.AdultCount);
        }
    }
}
