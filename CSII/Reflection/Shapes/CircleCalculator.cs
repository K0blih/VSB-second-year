namespace Shapes
{
	public class CircleCalculator
	{
		public double Radius { get; set; }

		public double GetArea()
		{
			return Math.PI * Math.Pow(Radius, 2);
		}

		public string GetArea(string units)
		{
			return GetArea() + units;
		}
	}
}
