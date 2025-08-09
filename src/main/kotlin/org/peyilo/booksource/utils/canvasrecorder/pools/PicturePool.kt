package org.peyilo.booksource.utils.canvasrecorder.pools

import android.graphics.Picture
import org.peyilo.booksource.utils.objectpool.BaseObjectPool

class PicturePool : BaseObjectPool<Picture>(64) {

    override fun create(): Picture = Picture()

}
