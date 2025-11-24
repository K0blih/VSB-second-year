using cs1_cv8;
using cs1_cv8.Filters;
using cs1_cv8.Products;

namespace cs1_cv8Main
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Stock<IProduct> stock = new Stock<IProduct>();
            Voucher voucher = new Voucher() { Name = "test", Price = 2300 };

            stock.Add(voucher);
            stock.Add(new Car() { Name = "Auto", Price = 1_000_000, Width = 200, Height = 150, Length = 300, WheelCount = 4 });
            stock.Add(new MobilePhone() { Name = "Mobil", Price = 18_000, Width = 6, Height = 16, Length = 1 });

            FilterEvaluator filterEvaluator = new FilterEvaluator();
            filterEvaluator.Add(new WidthFilter() { Max = 300 });
            filterEvaluator.Add(new WheelCountFilter() { WheelCount_ = 4 });

            foreach (IProduct product in stock.Filter(filterEvaluator))
            {
                Console.WriteLine(product.Name);
            }

            //try
            //{
            //    stock.Add(voucher);
            //    stock.Add(voucher);
            //}
            //catch (DuplicitProductException e)
            //{
            //    Console.WriteLine("Nepodarilo se pridat produkt! " + e.Product.Name);
            //}
        }
    }
}
