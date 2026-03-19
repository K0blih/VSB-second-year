namespace cv5.Models
{
    public class ProductService
    {
        public List<Product> List()
        {
            return Product.GetProducts();
        }

        public Product GetProduct(int id)
        {
            List<Product> products = Product.GetProducts();
            return products[id - 1];
        }
    }
}
