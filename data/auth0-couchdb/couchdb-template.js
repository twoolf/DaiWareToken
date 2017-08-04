//We'll try to use the MongoDB template as a base model.

function login (email, password, callback) {
  mongo('mongodb://user:pass@mymongoserver.com/my-db', function (db) {
    var users = db.collection('users');
    users.findOne({email: email}, function (err, user) {
  
      if (err) return callback(err);
  
      if (!user) return callback();
  
      if (!bcrypt.compareSync(password, user.password)) {
        return callback();
      }
  
      callback(null,   {
        user_id:      user._id.toString(),
        nickname:     user.nickname,
        email:        user.email
      });
  
    });  
  });
}
