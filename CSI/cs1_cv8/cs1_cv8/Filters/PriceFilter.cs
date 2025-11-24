using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv8.Filters
{
    public class PriceFilter : IFilter
    {
        public double? Min { get; set; }
        public double? Max { get; set; }

        public bool IsMatch(IProduct product)
        {
            if (Min == null && Max == null)
            {
                throw new InvalidFilterArgumentsException("Spatne nastaveny mix a max.");
            }

            if (product.Price > Max)
            {
                return false;
            }
            if (product.Price < Min)
            {
                return false;
            }

            return true;
        }
    }
}
