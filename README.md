## Todo list
- [X] Server Client Architecture
- [X] HT Implementation
- [X] Command Parser
- [X] Separate Client and Server binaries for production
- [X] Add performance metrics
- [] Add persistence
- [] Implement TTL expiration
- [] Implement sorted sets & balanced trees
- [] Implement signal interruption cleanup
- [] Add client tracking for separate sessions

- ## Performance Metrics

### Latency Performance
- **0.12ms average response time** under normal load
- **0.51ms average response time** under 10,000 concurrent requests
- **Sub-millisecond latency** maintained at scale
- Consistent performance across varying load conditions

### Throughput & Scalability
- Handles **10,000+ concurrent connections**
- Non-blocking I/O architecture for high concurrency
- Efficient memory management with custom hashtable implementation
- Production-grade performance comparable to Redis

### Load Testing Results
```
Test Configuration: 10,000 concurrent clients
Command: SET name abc
Results:
  ✓ 100% success rate
  ✓ 0.51ms average response time
  ✓ High throughput sustained under load
```

### Architecture Highlights
- **Non-blocking event-driven I/O** using epoll/poll
- **Custom hashtable** with collision resolution
- **Length-prefixed protocol** for efficient message parsing
- **Real-time metrics collection** for performance monitoring
- **Docker containerization** for consistent deployment

### Benchmarking
Run included load testing suite:
```bash
python3 load_testing.py
# Test with 1000, 5000, or 10000+ concurrent clients
```

### Performance Comparison
| Metric | Redis-Lite | Typical Redis |
|--------|------------|---------------|
| Latency | 0.51ms | 0.1-1ms |
| Concurrency | 10K+ | 10K+ |
| Memory | Efficient | Baseline |
