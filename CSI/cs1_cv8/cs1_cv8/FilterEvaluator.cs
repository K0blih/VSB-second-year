using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs1_cv8
{
    public class FilterEvaluator
    {
        private List<IFilter> filters = new List<IFilter>();

        public void Add(IFilter filter)
        {
            filters.Add(filter);
        }

        public bool isMatch(IProduct product)
        {
            foreach(IFilter filter in filters)
            {
                if (!filter.IsMatch(product))
                {
                    return false;
                }
            }
            return true;
        }
    }
}
