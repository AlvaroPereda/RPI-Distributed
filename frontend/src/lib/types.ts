export type Metrics = {
    predicted_per_second: number,
    predicted_ms: number,
    predicted_n: number
}

export type Prompt = {
    role: "user" | "assistant" | "system",
    content: string,
    metrics?: Metrics
}

export type LlamaAPI = {
    choices: {
        delta: {
            content: string
        },
        finish_reason: string
    }[],
    model: string,
    timings: Metrics
}

export type Device = {
    name: string,
    ip: string,
    password?: string
}