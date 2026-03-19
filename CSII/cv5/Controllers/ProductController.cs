using cv5.Models;
using Microsoft.AspNetCore.Mvc;

namespace cv5.Controllers
{
    public class ProductController : Controller
    {
        [HttpGet]
        public IActionResult Index(ProductService productService)
        {
            ViewBag.Products = productService.List();
            return View();
        }

        [HttpGet]
        public IActionResult Detail( [FromRoute] int id, [FromServices] ProductService productService) // [FromQuery] string test
        {
            Product product = productService.GetProduct(id);
            if(product == null)
            {
                return NotFound();
            }
            ViewBag.Product = product;
            return View(new AddToCartForm()
            {
                ProductId = id,
                Quantity = 3
            });
        }

        [HttpPost]
        public IActionResult Detail(
            [FromRoute] int id,
            [FromForm] AddToCartForm form,
            [FromServices] ProductService productService,
            [FromServices] CartService cartService
            ) // [FromQuery] string test
        {
            Product product = productService.GetProduct(id);
            if (product == null)
            {
                return NotFound();
            }

            if (ModelState.IsValid)
            {
                int quantity = form.Quantity.GetValueOrDefault();

                if (quantity == 2)
                {
                    ModelState.AddModelError("Quantity", "Nesmi byt 2!");
                }

                if (ModelState.IsValid)
                {
                    for (int i = 0; i < quantity; i++)
                    {
                        cartService.Add(product);
                    }

                    return RedirectToAction("Cart");
                }
            }

            ViewBag.Product = product;
            return View(new AddToCartForm()
            {
                ProductId = id,
                Quantity = 3
            });
        }

        [HttpGet]
        public IActionResult Cart([FromServices] CartService cartService)
        {
            ViewBag.Items = cartService.List();
            return View();
        }
    }
}
