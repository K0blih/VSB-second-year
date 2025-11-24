using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv8.Filters
{
    public class WheelCountFilter : IFilter
    {
        public int WheelCount_ { get; set; }

        public bool IsMatch(IProduct product)
        {
            if (product is IWheeledVehicle vehicle)
            {
                if (vehicle.WheelCount == WheelCount_)
                {
                    return true;
                }
            }
            return false;
        }
    }
}
