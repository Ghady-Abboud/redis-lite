FROM alpine:latest
RUN apk add --no-cache gcc musl-dev make
WORKDIR /app
COPY . .
RUN make redis-server
EXPOSE 6379 
CMD ["./redis-server"]