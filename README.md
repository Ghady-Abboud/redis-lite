[X] Server Client Architecture
[X] HT Implementation
[] Command Parser
[] Implement TTL expiration
[] Implement sorted sets & balanced trees
[] Implement signal interruption cleanup
[] Add Client Authentication
[] Separate Client and Server binaries for production



## 🚀 Deployment Notes
- Single-user server (no multi-tenant isolation)
- Deploy separate instances for different users
- Suitable for development, caching, single-application use


## ⚠️ Security Note
This is a **development/learning server**. 

**DO NOT use for production!**

- All connected clients share the same key space
- No authentication or user isolation

For production: Deploy separate instances per user/application.