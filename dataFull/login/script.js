var current = null;
document.querySelector('#email').addEventListener('focus', function (e) {
  if (current) current.pause();
  current = anime({
    targets: 'path',
    strokeDashoffset: {
      value: 0,
      duration: 700,
      easing: 'easeOutQuart'
    },
    strokeDasharray: {
      value: '240 1386',
      duration: 700,
      easing: 'easeOutQuart'
    }
  });

  if (!document.getElementById('emailLabel').classList.contains("labelTop1")) {
    if (document.getElementById('emailLabel').classList.contains('labelBottom1'))
      document.getElementById('emailLabel').classList.remove('labelBottom1');

    document.getElementById('emailLabel').classList.add('labelTop1');
  }
});
document.querySelector('#email').addEventListener('focusout', function (e) {
  if (document.querySelector('#email').value.length == 0) {
    document.getElementById('emailLabel').classList.remove('labelTop1');
    document.getElementById('emailLabel').classList.add('labelBottom1');
  }
});

document.querySelector('#password').addEventListener('focus', function (e) {
  if (current) current.pause();
  current = anime({
    targets: 'path',
    strokeDashoffset: {
      value: -336,
      duration: 700,
      easing: 'easeOutQuart'
    },
    strokeDasharray: {
      value: '240 1386',
      duration: 700,
      easing: 'easeOutQuart'
    }
  });

  if (!document.getElementById('passwordLabel').classList.contains("labelTop2")) {
    if (document.getElementById('passwordLabel').classList.contains('labelBottom2'))
      document.getElementById('passwordLabel').classList.remove('labelBottom2');

    document.getElementById('passwordLabel').classList.add('labelTop2');
  }
});
document.querySelector('#password').addEventListener('focusout', function (e) {
  if (document.querySelector('#password').value.length == 0){
    document.getElementById('passwordLabel').classList.remove('labelTop2');
    document.getElementById('passwordLabel').classList.add('labelBottom2');
  }
});

document.querySelector('#submit').addEventListener('focus', function (e) {
  if (current) current.pause();
  current = anime({
    targets: 'path',
    strokeDashoffset: {
      value: -730,
      duration: 700,
      easing: 'easeOutQuart'
    },
    strokeDasharray: {
      value: '530 1386',
      duration: 700,
      easing: 'easeOutQuart'
    }
  });
});