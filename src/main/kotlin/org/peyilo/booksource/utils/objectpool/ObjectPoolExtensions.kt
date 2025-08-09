package org.peyilo.booksource.utils.objectpool

fun <T> ObjectPool<T>.synchronized(): ObjectPool<T> = ObjectPoolLocked(this)
