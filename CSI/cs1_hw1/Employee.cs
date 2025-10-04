using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_hw1
{
    internal class Employee
    {
        public string Name { get; }
        public int? Age { get; }
        public PhoneNumber? PhoneNumber { get; }
        public int Salary { get; }
        public bool? IsActive { get; }

        public Employee(string name, int? age, PhoneNumber? phoneNumber, int salary, bool? isActive)
        {
            Name = name;
            Age = age;
            PhoneNumber = phoneNumber;
            Salary = salary;
            IsActive = isActive;
        }
    }

    internal struct PhoneNumber
    {
        public string CountryCode { get; }
        public long NationalNumber { get; }

        public PhoneNumber(string countryCode, long nationalNumber)
        {
            CountryCode = countryCode;
            NationalNumber = nationalNumber;
        }

        public override string ToString()
        {
            return $"{CountryCode}{NationalNumber}";
        }
    }
}
