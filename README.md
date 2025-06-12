[X] Server Client Architecture
[X] HT Implementation
[] Command Parser
[] Separate Client and Server binaries for production
[] Implement TTL expiration
[] Implement sorted sets & balanced trees
[] Implement signal interruption cleanup
[] Add client tracking for separate sessions 



## 🚀 Deployment Notes
- Single-user server 
- Deploy separate instances for different users
- Suitable for development, caching, single-application use


## ⚠️ Security Note
This is a **development/learning server**. 

**DO NOT use for production!**

- All connected clients share the same key space
- No authentication or user isolation

For production: Deploy separate instances per user/application.