package org.peyilo.booksource.utils.objectpool

interface ObjectPool<T> {

    fun obtain(): T

    fun recycle(target: T)

    fun create(): T

}
