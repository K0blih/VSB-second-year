using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv8.Filters
{
    public class WidthFilter : IFilter
    {
        public double? Min { get; set; }
        public double? Max { get; set; }
        public bool IsMatch(IProduct product)
        {
            if (Min == null && Max == null)
            {
                throw new InvalidFilterArgumentsException("Spatne nastaveny mix a max.");
            }

            if (product is IPhysicalProduct prod)
            {
                if (prod.Width > Max)
                {
                    return false;
                }
                if (prod.Height < Min)
                {
                    return false;
                }
            }

            return true;
        }
    }
}
