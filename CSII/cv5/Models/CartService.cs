using System.Text.Json;

namespace cv5.Models
{
    public class CartService
    {
        private readonly IHttpContextAccessor accessor;

        public CartService(IHttpContextAccessor accessor)
        {
            this.accessor = accessor;
        }

        public List<Product> List()
        {
            HttpContext ctx = accessor.HttpContext;
            string json = ctx.Session.GetString("cart") ?? "[]";
            return JsonSerializer.Deserialize<List<Product>>(json);
        }

        public void Add(Product product)
        {
            List<Product> data = List();
            data.Add(product);

            HttpContext ctx = accessor.HttpContext;
            ctx.Session.SetString("cart", JsonSerializer.Serialize(data));
        }
    }
}
