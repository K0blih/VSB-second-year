namespace ProductsApp
{
    delegate string MyFirstDelegate(string x, int y);
    delegate bool Condition(int x);

    static class MyExtMethods
    {
        public static int Pow(this int number, int n)
        {
            return (int)Math.Pow(number, n);
        }

        public static IEnumerable<int> EvenPosOnly(this IEnumerable<int> arr)
        {
            int i = 0;
            foreach (int n in arr)
            {
                if (i % 2 == 0)
                {
                    yield return n;
                }
                i++;
            }
        }

        public static IEnumerable<int> Filter(this IEnumerable<int> arr, Condition cond)
        {
            foreach (int n in arr)
            {
                if (cond(n))
                {
                    yield return n;
                }
            }
        }
    }

    internal class Program
    {
        private static string Test(string x, int y)
        {
            Console.WriteLine("A");
            return $"{x}: {y}";
        }

        private static string Test2(string x, int y)
        {
            Console.WriteLine("B");
            return $"{x}: {y}";
        }

        static void Main(string[] args)
        {
            static int Sum(int x, int y)
            {
                return x + y;
            }

            MyFirstDelegate tmp = Test;
            tmp += Test2;
            tmp -= Test;

            //string result = tmp("Number", 5);
            string result = tmp?.Invoke("Number", 5);

            Console.WriteLine(result);

            Calculator calc = new Calculator();
            calc.OnSetXY += (sender, args) => Console.WriteLine("Nastaveno XY");
            calc.OnCompute += (sender, args) => Console.WriteLine("Vysledek: " + args.Result);

            calc.SetXY(10, 5);
            calc.Execute(Sum);
            //calc.Execute((x, y) => x + y);
            //calc.Execute((int x, int y) => x + y);
            //calc.Execute((int x, int y) =>
            //{
            //    return x + y;
            //});





            int x = 3;
            int num2 = MyExtMethods.Pow(x, 2);
            int num1 = x.Pow(2).Pow(3).Pow(4);
            Console.WriteLine(num1);

            int[] nums = [1, 2, 3, 4, 5, 6, 7];
            List<int> tmp2 = new List<int>(nums);
            Console.WriteLine(string.Join(", ", tmp2.EvenPosOnly()));
            Console.WriteLine(string.Join(", ", tmp2.Filter(x => x < 5).Filter(x => x > 1)));

            foreach(int i in tmp2.Where(x => x < 5).Where(x => x > 1))
            {
                Console.WriteLine(i);
                break;
            }
                
            

            IEnumerable<Product> products = GetProducts();

            double avg = products
                .Where(x => x.Price != null)
                .Select(x => x.Price.Value)
                .Where(x => x < 10000)
                .Average();
                //.Average(x => x.Price.Value);
            Console.WriteLine(avg);
        }



        private static IEnumerable<Product> GetProducts()
        {
            return new List<Product>()
            {
                new Product(){ Id = 1, Name = "Auto", Price = 700_000, Quantity = 10 },
                new Product(){ Id = 1, Name = "Slon", Price = 1_500_000, Quantity = 0 },
                new Product(){ Id = 1, Name = "Kolo", Price = 26_000, Quantity = 5 },
                new Product(){ Id = 1, Name = "Brusle", Price = 2_800, Quantity = 30 },
                new Product(){ Id = 1, Name = "Hodinky", Price = 18_500, Quantity = 2 },
                new Product(){ Id = 1, Name = "Mobil", Price = 24_000, Quantity = 0 }
            };
        }
    }
}
