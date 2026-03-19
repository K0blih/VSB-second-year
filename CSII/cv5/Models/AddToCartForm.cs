using System.ComponentModel;
using System.ComponentModel.DataAnnotations;

namespace cv5.Models
{
    public class AddToCartForm
    {
        [DisplayName("pocet kusu")]
        [Required]
        [Range(1, 10)]
        //[EmailAddress]
        //[MaxLength(100)]
        //[MinLength(2)]
        public int? Quantity { get; set; }
        public int ProductId { get; set; }


    }
}
