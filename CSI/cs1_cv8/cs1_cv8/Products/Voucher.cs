using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv8.Products
{
    public class Voucher : IProduct
    {
        public string Name { get; set; }
        public double Price { get; set; }
    }
}
