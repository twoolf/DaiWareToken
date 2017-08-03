import asyncio

from aiocache.utils import get_cache_value_with_fallbacks
from aiocache.backends import SimpleMemoryBackend


class LRUMemoryBackend(SimpleMemoryBackend):
    """
    Wrapper around Python 3.6.1 dict operations to use it as a cache backend

    WARNING: DO NOT USE IN PYTHON <3.6.1
    ====================================
    This relies on non-spec related implementation properties of dicts,
    namely that dicts are internally ordered in Python 3.6.1:
    https://t.co/du4P4M4LFN
    https://twitter.com/raymondh/status/773978885092323328
    https://twitter.com/raymondh/status/850102884972675072

    We can then use pop/insert to shuffle recently accessed items to the
    end of the dictionary.

    Note: an alternative implementation would use an OrderedDict, but that
    has historically been substantially slower than a builtin dict
    """

    DEFAULT_MAX_SIZE=10000

    def __init__(self, max_size=None, **kwargs):
        self.max_size = get_cache_value_with_fallbacks(
            max_size, from_config="max_size",
            from_fallback=self.DEFAULT_MAX_SIZE, cls=self.__class__)
        super().__init__(**kwargs)

    async def _evict(self):
        """
        Evicts the oldest entries as needed if the cache exceeds its max size.

        :returns: The number of keys evicted
        """
        if self.max_size > 0 and len(SimpleMemoryBackend._cache) > self.max_size:
            keys_to_evict = list(SimpleMemoryBackend._cache.keys())[:self.max_size]
            await asyncio.gather(*[self._delete(k) for k in keys_to_evict])
            return len(keys_to_evict)
        else:
            return 0

    async def _set(self, key, value, ttl=None):
        """
        Stores the value in the given key.
        We pop the existing kv-pair to move it to the end of the dict.

        :param key: str
        :param value: obj
        :param ttl: int
        :returns: True
        """
        # We must evict unless we successfully remove a key.
        # This is because we are guaranteed to insert the item with the
        # []-operator if the key does not already exist in the dict.
        deleted = await self._delete(key)

        if not deleted:
            await self._evict()

        return await super()._set(key, value, ttl=ttl)

    async def _add(self, key, value, ttl=None):
        """
        Stores the value in the given key. Raises an error if the
        key already exists.

        :param key: str
        :param value: obj
        :param ttl: int
        :returns: True if key is inserted
        :raises: Value error if key already exists
        """
        if key in SimpleMemoryBackend._cache:
            raise ValueError(
                "Key {} already exists, use .set to update the value".format(key))

        self._evict()

        await self._set(key, value, ttl=ttl)
        return True
