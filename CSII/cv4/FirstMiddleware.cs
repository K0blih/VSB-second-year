using System.ComponentModel;

namespace cv4
{
    public class FirstMiddleware
    {
        public FirstMiddleware(RequestDelegate next)
        {

        }

        public async Task Invoke(HttpContext ctx)
        {
            //string path = ctx.Request.Path;
            //await ctx.Response.WriteAsync(path);

            ctx.Response.Headers.ContentType = "text/html; charset=utf-8";

            await ctx.Response.WriteAsync(@"
                    <html>
                        <head>
                            <title>Nazev stranky</title>
                        </head>
                        <body>
                            <h1>Nadpis první stranky<h1>
                        </body>
                    </html>
                ");

        }
    }
}
