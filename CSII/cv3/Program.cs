namespace cv3
{
    internal class Program
    {
        //private static int result = 0;

        //private static void T1()
        //{
        //    result = 10;
        //}

        //static void Main(string[] args)
        //{
        //    //result = 1;

        //    //Thread thread = new Thread(T1);
        //    //thread.Start();

        //    //thread.IsBackground = false;

        //    //thread.Join(); ceka nez vlakno dobehne

        //    //Console.WriteLine(result);

        //    //SimpleStack<int> stack = new SimpleStack<int>();
        //    //Random rand = new Random();

        //    //for (int i = 0; i < 5; i++)
        //    //{
        //    //    Thread t = new Thread(() =>
        //    //    {
        //    //        while (true)
        //    //        {
        //    //            if (rand.NextDouble() < 0.8)
        //    //            {
        //    //                if (stack.TryPop(out int result))
        //    //                {
        //    //                    //int result = stack.Pop();

        //    //                    Console.WriteLine(result);
        //    //                }
        //    //            }
        //    //            else
        //    //            {
        //    //                stack.Push(rand.Next());
        //    //            }
        //    //        }
        //    //    });
        //    //    t.Start();
        //    //}

        //    object lockObject = new object();

        //    SimpleStack<int> stack = new SimpleStack<int>();
        //    Random rand = new Random();

        //    Thread producer = new Thread(() =>
        //    {
        //        while (true)
        //        {
        //            stack.Push(rand.Next());
        //            lock (lockObject)
        //            {
        //                Monitor.Pulse(lockObject);
        //            }
        //            Thread.Sleep(100);
        //        }
        //    });
        //    producer.Start();

        //    for (int i = 0; i < 5; i++)
        //    {
        //        Thread t = new Thread(() =>
        //        {
        //            while (true)
        //            {
        //                if (stack.TryPop(out int result))
        //                {
        //                    Console.WriteLine(result);
        //                }
        //                else
        //                {
        //                    Console.WriteLine("Zasobnik je prazdny");
        //                    lock (lockObject)
        //                    {
        //                        Monitor.Wait(lockObject);
        //                    }
        //                }
        //                //Thread.Sleep(rand.Next(40, 1000));
        //            }
        //        });
        //        t.Start();
        //    }
        //}

        static async Task Main(string[] args)
        {
            await Write();

            int result = await Load();
        }

        private static async Task Write()
        {
            using StreamWriter sw = new StreamWriter("data.txt");
            //Task t = sw.WriteAsync("10");
            //await t;

            await sw.WriteAsync("10");

            Console.WriteLine("Hotovo");
            //t.ContinueWith(t =>
            //{
            //    Console.WriteLine("Hotovo");
            //});
        }

        private static async Task<int> Load()
        {
            using StreamReader sr = new StreamReader("data.txt");
            string txt = await sr.ReadToEndAsync();

            return int.Parse(txt);
        }
    }
}
