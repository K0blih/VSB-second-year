namespace cs1_hw1
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string csv = File.ReadAllText(args[0]);

            Employee[] employees = Utility.ParseCSV(csv);

            double? average = Utility.AverageAge(employees, out int ignoredEmployees);
            Console.WriteLine($"Průměrný věk: {average}");
            Console.WriteLine($"Zaměstnanců s neznámým věkem: {ignoredEmployees}");
            Console.WriteLine();

            bool noMatches = true;
            foreach (Employee employee in employees)
            {
                if ((employee.IsActive == true) && (employee.Salary > 30000) && (employee.PhoneNumber?.CountryCode != "+421"))
                {   
                    Console.WriteLine($"{employee.Name} | {employee.PhoneNumber}");
                    noMatches = false;
                }
            }

            if (noMatches)
            {
                Console.WriteLine("Žádný zaměstnanec neodpovídá filtru.");
            }
        }
    }
}
