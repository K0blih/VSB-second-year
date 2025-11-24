namespace cs1_cv8
{
    public class Stock<T> where T : IProduct
    {
        private List<T> products = new List<T>();

        public void Add(T product)
        {
            foreach(T item in products)
            {
                if (object.Equals(item, product))
                {
                    throw new DuplicitProductException() { Product = product };
                }
            }
            products.Add(product);
        }

        public List<T> Filter(FilterEvaluator filterEvaluator)
        {
            List<T> result = new List<T>();
            foreach (T item in products)
            {
                if(filterEvaluator.isMatch(item))
                {
                    result.Add(item);
                }
            }

            return result;
        }
    }
}
