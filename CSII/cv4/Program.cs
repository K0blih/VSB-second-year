namespace cv4
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            builder.Services.AddScoped<ExceptionHandler>();
            builder.Services.AddScoped<IMyLogger, TxtLogger>();

            var app = builder.Build();

            //builder.Services.AddTransient<ExceptionHandler>();
            //builder.Services.AddSingleton<ExceptionHandler>();


            //app.UseStaticFiles();

            app.UseMiddleware<ErrorMiddleware>();
            app.UseMiddleware<FormMiddleware>();
            app.UseMiddleware<BrowserAuthMiddleware>();
            app.UseMiddleware<FileMiddleware>();
            app.UseMiddleware<FirstMiddleware>();

            //app.MapGet("/", () => "Hello World!");

            app.Run();
        }
    }
}
