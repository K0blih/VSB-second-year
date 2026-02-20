using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProductsApp
{
    delegate int Operation(int x, int y);

    public class ResultEventArgs : EventArgs
    {
        public int Result { get; set; }
    }


    internal class Calculator
    {
        private int x;
        private int y;

        public event EventHandler OnSetXY;
        public event EventHandler<ResultEventArgs> OnCompute;

        public void SetXY(int x, int y)
        {
            this.x = x;
            this.y = y;

            OnSetXY?.Invoke(this, new EventArgs());
        }

        public void Execute(Operation op)
        {
            int result = op.Invoke(x, y);
            OnCompute?.Invoke(this, new ResultEventArgs() { Result = result });

            Console.WriteLine(result);
        }
    }
}
