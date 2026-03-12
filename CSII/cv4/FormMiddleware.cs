using System.Net;

namespace cv4
{
    public class FormMiddleware
    {
        private readonly RequestDelegate next;

        public FormMiddleware(RequestDelegate next)
        {
            this.next = next;
        }

        public async Task Invoke(HttpContext ctx)
        {
            if (!ctx.Request.Path.StartsWithSegments("/form"))
            {
                await next(ctx);
                return;
            }

            ctx.Response.Headers.ContentType = "text/html; charset=utf-8";

            if (ctx.Request.Method == "POST")
            {
                var data = ctx.Request.Form;
                string name = data["jmeno"];
                await ctx.Response.WriteAsync(WebUtility.HtmlEncode(name));
            }

            await ctx.Response.WriteAsync(@"
                <form method=""post"">
                    <input name=""jmeno"" />
                    <button type=""submit"">odeslat</button>
                </form>
            ");
        }
    }
}
