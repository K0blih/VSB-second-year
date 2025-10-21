using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv6
{
    internal class Population
    {
        private int personCount = 0;
        private Person[] persons = new Person[20];

        public Population(int personCount)
        {
            persons = new Person[personCount];
        }

        public void Add(Person person)
        {
            if (persons.Length == personCount)
            {
                Array.Resize(ref persons, personCount + 10);
            }
            persons[personCount] = person;
            personCount++;
        }

        public void PrintUnemployed()
        {
            Console.WriteLine("Nezaměstnaní:");
            for (int i = 0; i < personCount; i++)
            {
                if (persons[i] is Unemployed)
                {
                    persons[i].PrintInfo();
                }
            }
        }

        public void PrintEmployeesWithSalary()
        {
            Console.WriteLine("Zaměstnanci s platem:");
            for (int i = 0; i < personCount; i++)
            {
                if (persons[i] is Employee e)
                {
                    e.PrintInfo();
                    Console.WriteLine(e.GetSalary());
                }
            }
        }

        public void GetPersonWithHighestSalary()
        {
            Employee? person = null;
            for (int i = 0; i < personCount; i++)
            {
                if (persons[i] is Employee e)
                {
                    if (person is null)
                    {
                        person = e;
                    }
                    else if (e > person)
                    {
                        person = e;
                    }
                }
            }

            Console.WriteLine("Zaměstnanec s nejvyšším platem:");
            person?.PrintInfo();
        }

        public void GetHorlySalaryEmployees()
        {
            Console.WriteLine("Zaměstnanci s hodinovým platem:");
            for (int i = 0; i < personCount; i++)
            {
                if (persons[i] is HourlySalaryEmployee hse)
                {
                    hse.PrintInfo();
                    Console.WriteLine(hse.GetSalary());
                }
            }
        }

        public void IncreaseHourlySalary(double increase)
        {
            for (int i = 0; i < personCount; i++)
            {
                if (persons[i] is HourlySalaryEmployee hse)
                {
                    hse.HourlySalary *= increase;
                }
            }
        }
    }
}
