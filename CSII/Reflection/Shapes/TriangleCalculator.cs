namespace Shapes
{
	public class TriangleCalculator
	{
		public double Base { get; set; }
		public double Height { get; set; }

		public double GetArea()
		{
			return (Base * Height) / 2.0;
		}

		public string GetArea(string units)
		{
			return GetArea() + units;
		}
	}
}
