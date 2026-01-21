FROM oven/bun:1 AS build

WORKDIR /app

COPY . .

RUN bun install --frozen-lockfile

RUN bun run build

FROM nginx:alpine AS runner

RUN rm -rf /usr/share/nginx/html/*
COPY --from=build /app/build /usr/share/nginx/html

RUN rm /etc/nginx/conf.d/default.conf
COPY nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80