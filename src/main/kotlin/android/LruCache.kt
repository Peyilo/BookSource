package android

/**
 * 轻量 LRU 缓存（不依赖 Android）
 * - 默认按“条目个数”限流
 * - 也可传 weigher 让每个条目有自定义权重（比如字节数），行为类似 Android LruCache.sizeOf()
 */
open class LruCache<K, V>(
    private val maxSize: Int,
    private val weigher: ((key: K, value: V) -> Int)? = null
) {
    init { require(maxSize > 0) }

    // accessOrder = true -> 访问会移动到尾部，实现 LRU
    private val map = object : LinkedHashMap<K, V>(16, 0.75f, true) {
        override fun removeEldestEntry(eldest: MutableMap.MutableEntry<K, V>?): Boolean {
            // 不能直接在这里驱逐，因为我们需要根据总权重/个数控制
            // 统一交给 trimToSize 处理
            return false
        }
    }

    private var currentSize = 0 // 条目数或总权重

    @Synchronized
    operator fun get(key: K): V? = map[key]

    @Synchronized
    fun put(key: K, value: V): V? {
        val old = map.put(key, value)
        currentSize += sizeOf(key, value)
        if (old != null) currentSize -= sizeOf(key, old)
        trimToSize(maxSize)
        return old
    }

    @Synchronized
    fun remove(key: K): V? {
        val old = map.remove(key)
        if (old != null) currentSize -= sizeOf(key, old)
        return old
    }

    @Synchronized
    fun clear() {
        map.clear()
        currentSize = 0
    }

    @Synchronized
    fun size(): Int = currentSize

    @Synchronized
    fun isEmpty(): Boolean = map.isEmpty()

    @Synchronized
    fun snapshot(): Map<K, V> = LinkedHashMap(map)

    private fun sizeOf(k: K, v: V): Int = weigher?.invoke(k, v) ?: 1

    private fun shouldTrim(): Boolean = currentSize > maxSize

    private fun trimToSize(targetSize: Int) {
        // 逐个驱逐最老的，直到满足限制
        val it = map.entries.iterator()
        while (currentSize > targetSize && it.hasNext()) {
            val e = it.next()
            currentSize -= sizeOf(e.key, e.value)
            it.remove()
        }
    }
}
