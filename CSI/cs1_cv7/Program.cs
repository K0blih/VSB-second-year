namespace cs1_cv7
{
    internal class Program
    {
        static void Main(string[] args)
        {
            int[] tmp = [1, 2, 3, 4];
            ArrayHelper<int>.Swap(tmp, 1, 2);

            string[] tmp2 = ["A", "B", "C"];
            ArrayHelper<string>.Swap(tmp2, 0, 2);

            foreach (var item in tmp2)
            {
                Console.WriteLine(item);
            }

            TreeMap<int, string> tree = new TreeMap<int, string>();
        }
    }
}
