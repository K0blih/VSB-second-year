using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace ReflectionApp
{
	class ColumnNameAttribute : Attribute
	{
		public string Name { get; }
		public ColumnNameAttribute(string name)
		{
			this.Name = name;
		}
	}
    class Person
    {
		[ColumnName("Jmeno")]
        public string Name { get; set; }
        public int Age { get; set; }
    }

    class Car
    {
        public int Speed { get; set; }
    }

    public class Program
	{
		public static string CreateInsertSQL<T>(T obj)
		{
			Type t = typeof(T);

			string tableName = t.Name;
			//string tableName = obj.GetType().Name;

			string sql = "INSERT INTO " + tableName + " (";
			sql += string.Join(", ", t.GetProperties().Select(x =>
			{
				ColumnNameAttribute attr = x.GetCustomAttribute<ColumnNameAttribute>();
				if (attr != null) { return attr.Name; }
				return x.Name;
			}));
			sql += ") VALUES (";
            sql += string.Join(", ", t.GetProperties().Select(x =>
			{
				object tmp = x.GetValue(obj);
				if (tmp.GetType() == typeof(string))
				{
					return $"\"{tmp}\"";
				}

				return tmp;
			}));
			sql += ")";
			return sql;
		}

		public static void Main(string[] args)
		{
			Console.WriteLine(CreateInsertSQL(new Person() { Name = "Jan", Age = 20 }));
            Console.WriteLine(CreateInsertSQL(new Car() { Speed = 100 }));



            string baseDir = Path.GetFullPath("../../../plugins/Debug/net9.0");
			string[] dlls = Directory.GetFiles(baseDir, "*.dll");

			// 1. nacist vsechny knihovny z dlls
			Assembly[] assemblies = dlls.Select(x => Assembly.LoadFile(x)).ToArray();

			// 2. ziskat vsechny typy z nactenych knihoven, ktera konci "calculator"
			Type[] types = assemblies.SelectMany(x => x.GetTypes()).Where(x => x.Name.EndsWith("Calculator")).ToArray();

			// 3. umoznit uzivateli zadat/vybrat index typu
			for (int i = 0;  i < types.Length; i++)
			{
				Console.WriteLine($"{i}. {types[i].Name}");
			}
			int index = int.Parse(Console.ReadLine());

			Type calcType = types[index];

			Assembly assembly = calcType.Assembly;

			//string rectangleDllPath = Path.Combine(baseDir, "Rectangle.dll");

			//Assembly assembly = Assembly.LoadFile(rectangleDllPath);

			//Type[] types = assembly.GetTypes();
			//foreach (Type type in types)
			//{
			//	Console.Write(type.FullName);
			//}

			//Type calcType = assembly.GetType("Rectangle.RectangleCalculator");
			
			//object instance = assembly.CreateInstance("Rectangle.RectangleCalculator");
			object instance = assembly.CreateInstance(calcType.FullName);
			//object instance = Activator.CreateInstance(calcType, new object[] { "Ahoj", 50 });

			foreach (PropertyInfo pi in calcType.GetProperties())
			{
				Console.WriteLine(pi.Name + ":");
				double tmp = double.Parse(Console.ReadLine());

				if (pi.PropertyType == typeof(double))
				{
					// ano je to double...
				}

				pi.SetValue(instance, tmp);

				double value = (double)pi.GetValue(instance);
			}

			MethodInfo method1 = calcType.GetMethod("GetArea", new Type[0]);
			MethodInfo method2 = calcType.GetMethod("GetArea", new Type[] { typeof(string)});

			double result1 = (double)method1.Invoke(instance, new object[0]);
            Console.WriteLine(result1);

            string result2 = (string)method2.Invoke(instance, new object[] { "cm2" });
			Console.WriteLine(result2);

            //foreach (MethodInfo mi in calcType.GetMethods())
            //{
            //	Console.WriteLine(mi.ReturnType + " " + mi.Name);

            //	foreach (ParameterInfo pi in mi.GetParameters())
            //	{
            //		Console.WriteLine(" - " + pi.ParameterType + " " + pi.Name);
            //	}
            //}

            //Console.WriteLine("--------------");

            //foreach (FieldInfo fi in calcType.GetFields(BindingFlags.Instance | BindingFlags.NonPublic))
            //{
            //	Console.WriteLine(fi.Name);
            //}

            //Console.WriteLine();
        }
    }
}