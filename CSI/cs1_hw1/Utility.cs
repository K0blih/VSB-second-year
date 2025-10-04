using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw1
{
    internal class Utility
    {
        public static PhoneNumber? ParsePhoneNumber(string input)
        {
            if (string.IsNullOrEmpty(input))
            {
                return null;
            }

            foreach (char c in input)
            {
                if (!(char.IsDigit(c) || c == '+'))
                {
                    return null;
                }
            }

            if (input.StartsWith('+'))
            {
                if (input.Length < 5)
                {
                    return null;
                }

                string countryCode = input.Substring(0, 4);
                string nationalString = input.Substring(4);

                if (long.TryParse(nationalString, out long nationalNumber))
                {
                    return new PhoneNumber(countryCode, nationalNumber);
                }
                return null;
            }
            else
            {
                string countryCode = "+420";

                if (long.TryParse(input, out long nationalNumber))
                {
                    return new PhoneNumber(countryCode, nationalNumber);
                }
                return null;
            }
        }

        public static Employee[] ParseCSV(string csv)
        {
            string[] nonParsedEmployees = csv.Trim().Split("\n");

            int employeeCount = nonParsedEmployees.Length - 1;
            Employee[] employees = new Employee[employeeCount];
            for (int i = 1; i < nonParsedEmployees.Length; i++)
            {
                // Jméno 0 string;Věk 1 int?;Telefon 2 PhoneNumber?;Plat 3 int;Je aktivní 4 bool?
                string[] parsedEmployee = nonParsedEmployees[i].Trim().Split(";");

                string name = parsedEmployee[0];
                int? age = null;
                if (int.TryParse(parsedEmployee[1], out int tempAge))
                {
                    age = tempAge;
                }
                PhoneNumber? phone = ParsePhoneNumber(parsedEmployee[2]);
                int salary = int.Parse(parsedEmployee[3]);
                bool? isActive = null;
                if (parsedEmployee[4] == "ano")
                {
                    isActive = true;
                }
                else if (parsedEmployee[4] == "ne")
                {
                    isActive = false;
                }

                employees[i - 1] = new Employee(name, age, phone, salary, isActive);
            }

            return employees;
        }

        public static double? AverageAge(Employee[] employees, out int ignoredEmployees)
        {
            double average = 0;
            ignoredEmployees = 0;

            foreach (Employee employee in employees)
            {
                if (employee.Age.HasValue)
                {
                    average += employee.Age.Value;
                }
                else
                {
                    ignoredEmployees++;
                }
            }

            int validEmployees = employees.Length - ignoredEmployees;

            return validEmployees > 0 ? (average / validEmployees) : null;
        }
    }
}
