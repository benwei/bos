#ifndef __HASH_H__
#define __HASH_H__


u32_t string_hash(const char * s);
inline u32_t long_hash(u32_t val, u32_t bits);
inline u32_t ptr_hash(void *ptr, u32_t bits);

#endif /* __HASH_H__ */
