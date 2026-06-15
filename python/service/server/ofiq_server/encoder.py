# from connexion.apps.flask_app import FlaskJSONEncoder
from connexion.jsonifier import Jsonifier #, JSONEncoder as ConnexJSONEncoder
import six

from ofiq_server.models.base_model_ import Model


class JSONEncoder(Jsonifier): # FlaskJSONEncoder
    include_nulls = False

    def dumps(self, data, **kwargs):
    # def dumps(self, data):
        if isinstance(data, Model):
            dikt = {}
            for attr, _ in six.iteritems(data.swagger_types):
                value = getattr(data, attr)
                if value is None and not self.include_nulls:
                    continue
                attr = data.attribute_map[attr]
                dikt[attr] = value
            jstr = str(dikt)
            jstr = jstr.replace("'", '"')
            return jstr
        return Jsonifier.dumps(self, data, **kwargs)  # FlaskJSONEncoder.default(self, o)
