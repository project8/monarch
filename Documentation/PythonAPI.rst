Python API Reference (monarch3)
================================

The ``monarch3`` Python module is a pybind11 binding of the Monarch3 C++
library.  It exposes the same read/write workflow as the C++ API using
Pythonic conventions: method and property names use ``snake_case``, raw
data buffers are returned as :class:`numpy.ndarray` views, and files can be
managed with Python's ``with`` statement.

See :doc:`UsageMonarch3Python` for workflow examples and usage guidance.

.. automodule:: monarch3
   :members:
   :undoc-members:
   :show-inheritance:
