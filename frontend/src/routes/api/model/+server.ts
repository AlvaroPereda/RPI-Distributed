import { json } from "@sveltejs/kit";
import type { RequestHandler } from "./$types.js";

export const POST:RequestHandler = async({request}) => {

    const model: string = await request.json();
    console.log(`El modelo seleccionado es: ${model}`);

    return json({result: "ok"}, {status: 200})
}