using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Database
{
    public class PopulationDatabase
    {
        private Person[] population = new Person[3];
        private int index = -1;
        public int Count
        {
            get
            {
                return index + 1;
            }
        }

        public int AdultCount
        {
            get
            {
                int count = 0;
                for (int i = 0; i < Count; i++)
                {
                    if (population[i].IsAdult) { count++; }
                }

                return count;
            }
        }
        
        public void Add(Person person)
        {
            index++;
            if (index == population.Length)
            {
                Person[] tmp = new Person[population.Length * 2];
                Array.Copy(population, tmp, population.Length);
                population = tmp;
            }

            population[index] = person;
        }

        public double? GetAverageAge()
        {
            double ageSum = 0;
            int count = 0;

            for (int i = 0; i < Count; i ++)
            {
                if(population[i].Age.HasValue)
                {
                    ageSum += population[i].Age.Value;
                    count++;
                }
            }

            if (count > 0)
            {
                return ageSum / (double)count;

            }
            return null;
        }
    }
}
